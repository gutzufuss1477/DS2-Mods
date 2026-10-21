"""Append hidden Chiral parts; preserve all existing roots and controller names."""
import struct
import numpy as np
from serialize_native_geometry import metadata, ref, mesh_bounds, bone_bounds
from native_bundle import ROOT

U = lambda n: struct.pack('<I', n)
NAMES = {'left-upper': 0x6c1a0001, 'right-upper': 0x6c1a0002,
         'left-sole': 0x6c1a0003, 'right-sole': 0x6c1a0004,
         'pants-body': 0x6c1a0005, 'pants-strips': 0x6c1a0006,
         'hanging-0': 0x6c1a0007, 'hanging-1': 0x6c1a0008}


def extend_model(group, model_id, root_ids, expanded_ids):
    d = metadata(group.info['id'], model_id)
    resource = group.edit_private_copy(model_id)
    roots = len(d['ModelPartResources'])
    expanded = len(d['ExpandedModelPartResources'])
    # Restrict to the observed all-present native root/expanded arrays.
    assert all(v for v in d['ModelPartResources']+d['ExpandedModelPartResources'])
    root_pattern, expanded_pattern = U(roots)+b'\1'*roots, U(expanded)+b'\1'*expanded
    root_at = resource.raw.find(root_pattern, 16)
    expanded_at = resource.raw.find(expanded_pattern, root_at+len(root_pattern))
    assert root_at == 16 and expanded_at >= root_at+len(root_pattern)
    resource.raw[expanded_at:expanded_at+len(expanded_pattern)] = U(expanded+len(expanded_ids))+b'\1'*(expanded+len(expanded_ids))
    resource.raw[root_at:root_at+len(root_pattern)] = U(roots+len(root_ids))+b'\1'*(roots+len(root_ids))
    resource.replace(U(d['ModelPartResourceNumMustBeAdded']),
                     U(d['ModelPartResourceNumMustBeAdded']+len(expanded_ids)))
    links = resource.links
    expanded_link = roots+int('Helper' in d)
    resource.links = (links[:roots]+[(None, i) for i in root_ids]+
                      links[roots:expanded_link+expanded]+[(None, i) for i in expanded_ids]+
                      links[expanded_link+expanded:])


def extend_controller(group, art_id, ordered_labels, parents=None, insert_at=None):
    d = metadata(group.info['id'], art_id)
    resource = group.edit_private_copy(art_id)
    old = d['ModelPartNames']
    names = [NAMES[label] for label in ordered_labels]
    assert not set(old) & set(names)
    parents = parents or [-1]*len(names)
    assert len(names) == len(parents)
    count = len(old)
    insert_at = count if insert_at is None else insert_at
    assert 0 <= insert_at <= count
    # Main-model controllers precede cover/face/hair/cloth controllers.
    # Insert at the main-model boundary, shifting all later parent references.
    new_names = old[:insert_at]+names+old[insert_at:]
    resource.replace(U(count)+b''.join(U(4)+U(v) for v in old),
                     U(count+len(names))+b''.join(U(4)+U(v) for v in new_names))
    indices = d['ParentControllerIndex']
    shifted = [v+len(names) if v >= insert_at else v for v in indices]
    new_indices = shifted[:insert_at]+parents+shifted[insert_at:]
    resource.replace(U(count)+struct.pack('<'+'i'*count, *indices),
                     U(count+len(names))+struct.pack('<'+'i'*(count+len(names)), *new_indices))
    assert struct.unpack_from('<II?', resource.raw, len(resource.raw)-9) == (
        d['ExpandedModelPartResourceNum'], d['NeedCreateChildEntitiesNum'], d['IsForceDisablePaint'])
    struct.pack_into('<I', resource.raw, len(resource.raw)-9, d['ExpandedModelPartResourceNum']+len(names))
    if insert_at < count:
        # Native RVA 0x34c4b0 passes each child component's compiled offset to
        # 0x33dc40. Moving names/parents alone leaves overlapping/unbound slots.
        entities = (d.get('CoverModelEntityResources', []) +
                    ([d['FacialRigModelEntityResource']] if 'FacialRigModelEntityResource' in d else []) +
                    d.get('CoverAndAnimModelEntityResources', []))
        seen = set()
        for entity in entities:
            gid, eid = ref(entity)
            assert gid == group.info['id']
            components = metadata(gid, eid)['EntityComponentResources']
            matches = [(g, i) for g, i in map(ref, components) if g == gid and
                       group.rows[i][1] in ('CoverModelComponentResource',
                                            'CoverAndAnimModelComponentResource',
                                            'FacialRigModelComponentResource')]
            assert len(matches) == 1
            _, index = matches[0]
            assert index not in seen
            seen.add(index)
            rebase_cover_record(group.edit_private_copy(index).raw,
                                metadata(gid, index), insert_at, len(names))
    return count


def rebase_cover_record(raw, meta, boundary, growth):
    """Change the scalar ControllerOffset, preserving all bone mappings."""
    assert len(raw) > 25 and raw[16] == 1
    cursor = 17  # UUID and present CoverModelResource reference.
    for key in ['LogicMappingInfo', 'RepMappingInfo']:
        entries = meta[key]
        expected = U(len(entries))+b''.join(
            struct.pack('<ii', e['OwnIndex'], e['ParentIndex']) for e in entries)
        assert raw[cursor:cursor+len(expected)] == expected, key
        cursor += len(expected)
    old = struct.unpack_from('<I', raw, cursor)[0]
    assert old == meta['ControllerOffset'] and old >= boundary
    struct.pack_into('<I', raw, cursor, old+growth)
    return dict(fieldOffset=cursor, old=old, new=old+growth)


def new_part(group, source, mesh_id, bounds_id, label, child=None, hidden=True):
    native = metadata(group.info['id'], source)
    d = group.clone(source, label+'/part')
    # Donors for these clones intentionally have no children.
    assert not native['ChildModelPartResources']
    d.remap({(None, ref(native['MeshResource'])[1]):(None, mesh_id),
             (None, ref(native['BoneBoundingBoxes'])[1]):(None, bounds_id)})
    assert struct.unpack_from('<I', d.raw, len(d.raw)-5)[0] == native['NameHash']
    struct.pack_into('<I', d.raw, len(d.raw)-5, NAMES[label])
    d.raw[-1] = int(hidden)
    if child is not None:
        child_count_at = len(d.raw)-69-4
        assert struct.unpack_from('<I', d.raw, child_count_at)[0] == 0
        d.raw[child_count_at:child_count_at+4] = U(1)+b'\1'
        d.links.append((None, child))
    return d


def extend_worn_and_body(groups, records):
    from boot_geometry import read_stream
    import json
    by_name = {r['label']:r for r in records}
    boots = groups[31126]
    parts = {}
    # A sole is a child of its upper, so all native damage/menu selections keep
    # the entire boot visible without relying on the special extra-part slots.
    for side, source_part, lod_source in [('left', 16640, 15523), ('right', 18546, 8554)]:
        for kind in ['sole', 'upper']:
            label = side+'-'+kind
            row = by_name[label]
            folder = ROOT/'staging/chiral-cuff-v3-geometry-v2'
            layout = json.loads((folder/(label+'.layout.json')).read_text())
            data = read_stream((folder/(label+'.stream.bin')).read_bytes(), layout)
            points = data['positions']
            lod = boots.clone(lod_source, label+'/lod')
            assert len(lod.links) == 3 and all(g is None for g, _ in lod.links)
            lod.links = [(None, row['mesh'])]*3
            mesh_bounds(lod, points)
            part_meta = metadata(31126, source_part)
            binding = metadata(31126, ref(metadata(31126, row['source'])['SkinnedMeshJointBindings'])[1])
            bb = bone_bounds(boots, ref(part_meta['BoneBoundingBoxes'])[1], binding,
                             points, data['bones'], data['weights'], label+'/part-bounds')
            child = parts[side+'-sole'].id if kind == 'upper' else None
            parts[label] = new_part(boots, source_part, lod.id, bb.id, label, child, kind == 'upper')
    labels = ['left-upper', 'left-sole', 'right-upper', 'right-sole']
    old_count = len(metadata(31126, 4064)['ModelPartNames'])
    extend_model(boots, 9366, [parts['left-upper'].id, parts['right-upper'].id],
                 [parts[n].id for n in labels])
    extend_controller(boots, 4064, labels, [-1, old_count, -1, old_count+2])
    body = groups[39987]
    body_parts = []
    for label, source_part, lod_source in [('pants-body', 134, 915), ('pants-strips', 89, 2255)]:
        row = by_name[label]
        data = np.load(ROOT/f'staging/full-pants-graft-v1/{label}.npz')
        lod = body.clone(lod_source, label+'/lod')
        assert all(g is None for g, _ in lod.links)
        lod.links = [(None, row['mesh'])]*len(lod.links)
        mesh_bounds(lod, data['positions'])
        binding = metadata(39987, ref(metadata(39987, row['source'])['SkinnedMeshJointBindings'])[1])
        bb = bone_bounds(body, ref(metadata(39987, source_part)['BoneBoundingBoxes'])[1],
                         binding, data['positions'], data['bones'], data['weights'], label+'/part-bounds')
        # Empty root from the strip donor avoids inheriting/hiding body attachments.
        part = new_part(body, 89, lod.id, bb.id, label, hidden=True)
        body_parts.append(part.id)
    extend_model(body, 1455, body_parts, body_parts)
    extend_controller(body, 0, ['pants-body', 'pants-strips'], insert_at=53)
    return dict(privateNames=NAMES, wornRootParts=[parts['left-upper'].id, parts['right-upper'].id],
                bodyRootParts=body_parts, allOriginalNamesPreserved=True,
                originalBodyChildrenPreserved=True, newBodyRootsHiddenByDefault=True,
                lodPolicy='Same accepted geometry at each original LOD distance for first test',
                runtimeBodyVisibilityPending=True)
