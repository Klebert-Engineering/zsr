package enumeration_test;

enum uint8 Enum {
    A, B, C = 0xff
};

struct Owner {
    Enum a;
};
