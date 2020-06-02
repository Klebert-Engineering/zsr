package structure_test;

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
 * D - Reflect functions with various return types
 */
struct d_res {
    int32 a;
};

struct d_struct {
    d_res a;

    function uint32 fun() {
        return 10;
    }

    function d_res fun2() {
        return a;
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

/** H **/

struct h_struct_a {
    string a;
};

struct h_struct_b {
    string a;
};

/** I **/

struct i_struct_a(int32 a) {};

struct i_struct_b {
    int32 a;
    i_struct_a(a) b;
};

/** J **/

struct j_struct {
    int32 a;
    int32 b if a > 0;
};

/** K **/

struct k_struct_a {
    int32 a;
};

struct k_struct_b {
    k_struct_a a[];
};

/** L - Lookup member type declared after parent type */

struct l_parent {
    l_member a;
};

struct l_member {
    int32 a;
};


/** M - Check field type information */

struct m_child {
    int32 a;
};

struct m_parent {
    int32 a;
    uint16 b;
    float32 c;
    string d;
    m_child e;
};
