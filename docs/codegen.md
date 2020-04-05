# CodeGen

CodeGen is what we use to generate code that would otherwise take long to type
or are better handled more abstractly.

## Reasoning

Many repeated patterns in Vulpes actually require handling of data that might
be overly verbose. CodeGen removes that concern by letting people define the
data. And it will convert it to code using purpose crafted presets.

It also allows us to specify metadata for each data entry so the data can be
reused and converted to multiple types of code seamlessly.

## Capabilities

Currently **CodeGen 1.2.0** is only capable of generating one type of code.
Signatures.

# Signatures

Signatures are patterns of bytes that we can use to identify pieces of code in
the binary that we're hooking.

The way we have to write them out in source files is quite verbose.

```cpp
static LiteSignature signature_fix_death_timer_framerate_dep = { "fix_death_timer_framerate_dep", 29, { 0x38, 0x1D, -1, -1, -1, -1, 0x74, 0x33, 0xA1, -1, -1, -1, -1, 0x38, 0x58, 0x02, 0x75, 0x29, 0x66, 0xA1, -1, -1, -1, -1, 0x66, 0x8B, 0xC8, 0x66, 0x40 } };
```

(We also need getters, validators, and a good way to share them across files.)

We can cut this down with macros, but it still will have to require the 0x and
the -1s. This also means we can't just grab a signature from the code and
quickly manually search it in memory. Because the standard structure for this
is:
```
38 1D ?? ?? ?? ?? 74 33 A1 ?? ?? ?? ?? 38 58 02 75 29 66 A1 ?? ?? ?? ?? 66 8B C8 66 40
```
vs the structure that we have in code:
```
0x38, 0x1D, -1, -1, -1, -1, 0x74, 0x33, 0xA1, -1, -1, -1, -1, 0x38, 0x58, 0x02, 0x75, 0x29, 0x66, 0xA1, -1, -1, -1, -1, 0x66, 0x8B, 0xC8, 0x66, 0x40
```

CodeGen signatures handle all of this even with the user writing only a minimum
of two lines.

## Definition


| key     | values          | effect                                          |
|---------|-----------------|-------------------------------------------------|
| name    | \<string\>      | This will be the given name in errors. sig_\<name\>() will also be how you retrieve the address this signature has found.|
| bytes   | Bytes in a `XX XX XX XX XX` pattern | These are the values the signature will search for in memory. `??` is a wildcard and is skipped in scans. These can be used to account for values or areas that aren't set in stone like pointers. |
| base    | \<name string\> | If the base key is given whatever signature's name matched will be used to retrieve the value for this signature. The bytes are left unused. |
| offset  | \<int\>         | This number will be added to the output if an address was found. (Negative offsets are valid) Default: `0`.|
| type    | \<type name\>   | The type that the signature getter will output. It converts the value to this type with a reinterpret_cast. Default: `uintptr_t`.|
| crucial | \<boolean\>     | Whether or not this signature is absolutely required. If true execution will halt after validation finds out that it was not found. Default `false`.|
