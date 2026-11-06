#!/usr/bin/env python3
"""
Normalizes file modification times (mtime) of all source files in src/
to a fixed epoch (1730000000) so Ninja's dependency scanner recognizes
pre-compiled .obj files in the cache as strictly up-to-date and avoids
unnecessary recompilation after fresh git checkouts / gclient syncs.
"""

import os
import sys
import time

def normalize_tree(base_path, epoch=1730000000):
    if not os.path.exists(base_path):
        print(f"[normalize_mtime] Directory '{base_path}' not found, skipping.")
        return 0

    t0 = time.time()
    count = 0
    skipped_dirs = {'out', '.git', '.cipd'}

    for root, dirs, files in os.walk(base_path):
        # Prune output and metadata directories in-place
        dirs[:] = [d for d in dirs if d not in skipped_dirs]
        for fname in files:
            full_path = os.path.join(root, fname)
            try:
                os.utime(full_path, (epoch, epoch))
                count += 1
            except Exception:
                pass

    t1 = time.time()
    print(f"[normalize_mtime] Successfully normalized {count} source files in '{base_path}' in {t1 - t0:.2f}s")
    return count

if __name__ == '__main__':
    target = sys.argv[1] if len(sys.argv) > 1 else 'src'
    normalize_tree(target)
