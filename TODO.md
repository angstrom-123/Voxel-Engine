# To-Do List
- Fix drawing order (transparent blocks last)
- Frustum culling chunks 
- Find a less hacky way to fix the scaling issue
    - Currentlty fixed by making base quad 2x2, and halving the translation of each chunk at render

# Notes
- May not need to track chunk visibility in the struct anymore as it depends on bucket.
- Duplicate chunk in hashmap is likely placed there mistakenly, not a bug with the iterator.

- MIGHT HAVE TO MEMSET NULL WHENEVER REMOVING AN ENTRY (EVEN WHEN NOT FREEING IT MYSELF)
