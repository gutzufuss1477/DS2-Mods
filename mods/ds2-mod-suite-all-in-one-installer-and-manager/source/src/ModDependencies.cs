using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace DS2ModSuite
{
    internal static class ModDependencies
    {
        public static void ValidateCatalog(Catalog catalog)
        {
            var byId = catalog.Mods.ToDictionary(m => m.Id, StringComparer.OrdinalIgnoreCase);
            var visited = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            foreach (var mod in catalog.Mods)
                Visit(mod, byId, visited, new HashSet<string>(StringComparer.OrdinalIgnoreCase));
        }

        private static void Visit(ModSpec mod, Dictionary<string, ModSpec> byId,
            HashSet<string> visited, HashSet<string> active)
        {
            if (visited.Contains(mod.Id)) return;
            if (!active.Add(mod.Id)) throw new InvalidDataException("Cyclic mod dependency: " + mod.Id);
            var unique = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            foreach (string id in mod.Requires ?? new List<string>())
            {
                ModSpec dependency;
                if (string.IsNullOrWhiteSpace(id) || !unique.Add(id) || !byId.TryGetValue(id, out dependency)
                    || !string.IsNullOrWhiteSpace(dependency.ExclusiveGroup))
                    throw new InvalidDataException("Invalid mod dependency: " + mod.Id);
                Visit(dependency, byId, visited, active);
            }
            active.Remove(mod.Id);
            visited.Add(mod.Id);
        }

        public static string SelectionError(Catalog catalog, IEnumerable<string> ids)
        {
            var selected = new HashSet<string>(ids ?? Enumerable.Empty<string>(), StringComparer.OrdinalIgnoreCase);
            foreach (var mod in catalog.Mods.Where(m => selected.Contains(m.Id)))
                foreach (string required in mod.Requires ?? new List<string>())
                    if (!selected.Contains(required))
                        return Localization.Format("{0} requires {1}.", "{0} benötigt {1}.", mod.Name,
                            catalog.Mods.Single(m => string.Equals(m.Id, required, StringComparison.OrdinalIgnoreCase)).Name);
            return null;
        }

        // Called for explicit user selection. Removing a base also removes its dependents.
        public static void SetSelection(IList<ModRuntimeState> states, ModRuntimeState changed, bool enabled)
        {
            changed.DesiredEnabled = enabled;
            if (enabled && !string.IsNullOrWhiteSpace(changed.Spec.ExclusiveGroup))
                foreach (var other in states.Where(s => s != changed
                    && string.Equals(s.Spec.ExclusiveGroup, changed.Spec.ExclusiveGroup, StringComparison.OrdinalIgnoreCase)))
                    other.DesiredEnabled = false;
            if (enabled) IncludeRequired(states);
            else
            {
                bool removed;
                do
                {
                    removed = false;
                    foreach (var state in states.Where(s => s.DesiredEnabled))
                        if ((state.Spec.Requires ?? new List<string>()).Any(id => !states.Any(s =>
                            s.DesiredEnabled && string.Equals(s.Spec.Id, id, StringComparison.OrdinalIgnoreCase))))
                        { state.DesiredEnabled = false; removed = true; }
                } while (removed);
            }
        }

        public static void IncludeRequired(IList<ModRuntimeState> states)
        {
            var byId = states.ToDictionary(s => s.Spec.Id, StringComparer.OrdinalIgnoreCase);
            bool added;
            do
            {
                added = false;
                foreach (var state in states.Where(s => s.DesiredEnabled))
                    foreach (string id in state.Spec.Requires ?? new List<string>())
                    {
                        var dependency = byId[id];
                        if (!dependency.DesiredEnabled) { dependency.DesiredEnabled = true; added = true; }
                    }
            } while (added);
        }
    }
}
