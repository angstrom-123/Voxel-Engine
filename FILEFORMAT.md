# Format of a chunk file (Binary):
## Header 
- Version                   [ 2 Bytes  ]
- Size                      [ 2 Bytes  ]
- Subchunk Offset 0         [ 2 Bytes  ]
- Subchunk Offset 1         [ 2 Bytes  ]
- Subchunk Offset 2         [ 2 Bytes  ]
- Subchunk Offset 3         [ 2 Bytes  ]
- Subchunk Offset 4         [ 2 Bytes  ]
- Subchunk Offset 5         [ 2 Bytes  ]
- Subchunk Offset 6         [ 2 Bytes  ]
- Subchunk Offset 7         [ 2 Bytes  ]
- Subchunk End 0            [ 2 Bytes  ]
- Subchunk End 1            [ 2 Bytes  ]
- Subchunk End 2            [ 2 Bytes  ]
- Subchunk End 3            [ 2 Bytes  ]
- Subchunk End 4            [ 2 Bytes  ]
- Subchunk End 5            [ 2 Bytes  ]
- Subchunk End 6            [ 2 Bytes  ]
- Subchunk End 7            [ 2 Bytes  ]

## Body 
- Subchunk Data             [ Variable ]

# Format of a chunk metadata file (Plaintext):
| Line No. |       Data      |
| 1        | Player Pos XYZ  | Floats stored to 5dp.
| 2        | Player Rot XYZW | Floats stored to 5dp.
| 3        | Seed            |
| 4        | Time of Day     |
