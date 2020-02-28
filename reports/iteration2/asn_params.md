# ASN Param parsing

```
field-0: 1
field-1: Sequence:
 field-0=1.2.840.10045.1.1
 field-1=6277101735386680763835789423207666416083908700390324961279

field-2: Sequence:
 field-0=0xfffffffffffffffffffffffffffffffefffffffffffffffc
 field-1=0x64210519e59c80e70fa7e9ab72243049feb8deecc146b9b1
 field-2=275585503993527016686210752207080241786546919125

field-3: ¨°0ö|¿ ëC¡ôÿ
ýÿ+ÿÈÚxcík$ÍÕsùw¡yH
field-4: 6277101735386680763835789423176059013767194773182842284081
field-5: 1
```

*   `field-0`
*   `field-1`: field parameters
    *   `field-0`: some asn? TODO
    *   `field-1`: prime for the field
*   `field-2`
    *   `field-0`: A for the curve
    *   `field-1`: B for the curve
    *   `field-2`: Seed (wtf?) TODO
*   `field-3`: uncompressed generator
*   `field-4`: group order
*   `field-5`: cofactor
