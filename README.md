# AntBMS Monitor

## Schematics

```

│                ANT-BMS                 │
│                                        │
│  Comm                            Temp  │
└─[oooo]──[oooooooo]──[oooooooo]──[oooo]─┘
   ││││
   ││││
   ││││
   │││└─ TXD
   ││└── RXD 
   │└─── GND
   └──── VCC (3.3V)

## References

* https://github.com/klotztech/VBMS/wiki/Serial-protocol