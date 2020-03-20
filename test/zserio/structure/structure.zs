package structure;

/**
 * A
 */
struct a_struct {};

/**
 * B
 */
struct b_struct(uint32 a) {
};

/**
 * C
 */
struct c_parameter_struct {
    string a;
};

struct c_struct(c_parameter_struct a) {};

/**
 * D
 */
struct d_struct {
    function uint32 fun() {
        return 10;
    }
};


/**
 * E
 */
struct e_struct {
    string a;
};

/**
 * F
 */
struct f_child {
    int32 a;
};

struct f_parent {
    f_child a;

    function int32 child_value() {
        return a.a;
    }
};
