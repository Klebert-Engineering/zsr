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

/**
 * G
 */
struct g_struct {
    string a[];
};

/**
 * H
 */
struct h_struct_a {
    string a;
};

struct h_struct_b {
    string a;
};

/**
 * I
 */
struct i_struct_a(int32 a) {};

struct i_struct_b {
    int32 a;
    i_struct_a(a) b;
};
