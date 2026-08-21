#!/usr/bin/env python3
"""Reference tests for Infrastructure One Unit v1.0.0."""

MAX_VALUE = 2_000_000


def apply_policy(need, current, live, remaining=1):
    """Mirror the release policy used by the ASI.

    Current is the stage-local progress exposed to the UI/validation path.
    Live is the persisted local/cumulative progress on the infrastructure object.
    Existing online/other-player progress can therefore make current > live.
    """
    assert 1 <= remaining <= 1_000_000
    out_current = list(current)
    out_live = list(live)
    for i, (required, cur, persisted) in enumerate(zip(need, current, live)):
        assert 0 <= required <= MAX_VALUE
        assert 0 <= cur <= required
        assert 0 <= persisted <= MAX_VALUE
        if required - cur > remaining:
            target_current = required - remaining
            delta = target_current - cur
            target_live = persisted + delta
            assert delta > 0
            assert 0 <= target_live <= MAX_VALUE
            out_current[i] = target_current
            out_live[i] = target_live
    return out_live, out_current


def main():
    # Road construction with partial online/other-player contributions.
    need = [1600, 0, 2000, 2560, 0, 0]
    current = [1280, 0, 1600, 2040, 0, 0]
    live = [0, 0, 0, 0, 0, 0]
    assert apply_policy(need, current, live) == (
        [319, 0, 399, 519, 0, 0],
        [1599, 0, 1999, 2559, 0, 0],
    )

    # Road construction with local ceramic progress already represented in live.
    need = [1700, 0, 5200, 4480, 0, 0]
    current = [0, 0, 0, 4400, 0, 0]
    live = [0, 0, 0, 4400, 0, 0]
    assert apply_policy(need, current, live) == (
        [1699, 0, 5199, 4479, 0, 0],
        [1699, 0, 5199, 4479, 0, 0],
    )

    # Road stage 2. Live is cumulative while current is stage-local.
    need = [900, 0, 0, 680, 1360, 0]
    current = [900, 0, 0, 0, 0, 0]
    live = [2600, 0, 5200, 4480, 0, 0]
    assert apply_policy(need, current, live) == (
        [2600, 0, 5200, 5159, 1359, 0],
        [900, 0, 0, 679, 1359, 0],
    )

    # Monorail construction.
    assert apply_policy(
        [800, 1440, 0, 0, 0, 1560],
        [0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0],
    ) == (
        [799, 1439, 0, 0, 0, 1559],
        [799, 1439, 0, 0, 0, 1559],
    )

    # Mine construction and cumulative upgrades.
    assert apply_policy(
        [0, 1120, 0, 0, 0, 1320],
        [0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0],
    )[0] == [0, 1119, 0, 0, 0, 1319]

    assert apply_policy(
        [0, 2240, 0, 0, 0, 2400],
        [0, 0, 0, 0, 0, 0],
        [0, 1120, 0, 0, 0, 1320],
    )[0] == [0, 3359, 0, 0, 0, 3719]

    assert apply_policy(
        [0, 3360, 0, 0, 0, 3600],
        [0, 0, 0, 0, 0, 0],
        [0, 3360, 0, 0, 0, 3720],
    )[0] == [0, 6719, 0, 0, 0, 7319]

    # Custom INI value: never increase a requirement that is already below it.
    need = [3000, 80, 1, 100]
    current = [0, 0, 0, 100]
    live = [0, 0, 0, 100]
    assert apply_policy(need, current, live, remaining=50) == (
        [2950, 30, 0, 100],
        [2950, 30, 0, 100],
    )

    # Completed and already-at-limit slots are idempotent.
    assert apply_policy([100, 100], [100, 99], [250, 249]) == ([250, 249], [100, 99])
    print("PASS: direct, cumulative, online-contribution and custom RemainingUnits policies")


if __name__ == "__main__":
    main()
