# Technical notes

## Material vector

The game uses six signed 32-bit material slots. The mod does not hardcode which material category occupies a slot; it applies the policy only to positive still-open slots.

## Direct and cumulative progress

The same delta policy supports both direct construction and cumulative upgrade progress:

- direct build: `Live` usually equals local progress;
- upgrades: `Live` includes the basis of previous stages;
- online contributions: `Current` can exceed `Live`.

Adding only the missing delta preserves all three representations.

## Explicitly out of scope

- repairs;
- fabrication;
- cargo ownership and container lifetime;
- weight calculations;
- direct save-file editing.
