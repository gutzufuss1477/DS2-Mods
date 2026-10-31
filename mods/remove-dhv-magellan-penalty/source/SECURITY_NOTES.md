# Runtime scope and compatibility — v1.1.0

The x64 ASI is restricted to Steam PC DS2.exe build 1.10.89.0. It checks the PE timestamp, image size and 16 instruction signatures before enabling hooks. An unsupported or modified executable fails closed.

It adjusts the normal Magellan Likes lookup argument and selectively prevents the Magellan-specific evaluation override from replacing the game's native calculated rank. It never sets a fixed or maximum grade, does not grant arbitrary Likes, and does not edit Steam achievements.

The game continues to compute and save awards normally. This release copies two files beside DS2.exe; it does not overwrite game assets or rewrite savegames. Disabling/uninstalling the mod restores vanilla code on the next game start; already earned awards remain in the save.

The multiple software breakpoint hooks may conflict with other ASIs operating on identical instructions. Compatibility with all mod combinations and delivery types has not been established. Save backups are recommended before modifying game behavior.
