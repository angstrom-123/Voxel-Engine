# Format of a chunk file (Binary):
## Header 
- Version         [ 2 Bytes  ]
- Size            [ 2 Bytes  ]
- Subchunk Count  [ 1 Byte   ]

## Body 
- Subchunk
    - Index       [ 1 Byte   ]
    - Size        [ 2 Bytes  ]
    - Data        [ Variable ] RLE
- ...

# Format of a chunk metadata file (Plaintext):
| Line No. |       Data      |
| 1        | Player Pos XYZ  | Floats stored to 5dp.
| 2        | Player Rot XYZW | Floats stored to 5dp.
| 3        | Seed            |
| 4        | Time of Day     |
