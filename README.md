# AntBMS Monitor

## STM32F1XX series

Build and tested with STM32F100C8T6

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
```

## References

* https://github.com/klotztech/VBMS/wiki/Serial-protocol
