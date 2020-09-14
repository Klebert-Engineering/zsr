package structure_test;

/**
 * A
 */
struct A_struct {};

/**
 * B
 */
struct B_struct(uint32 a) {
};

/**
 * C
 */
struct C_parameter_struct {
    string a;
};

struct C_struct(C_parameter_struct a) {};

/**
 * D - Reflect functions with various return types
 */
struct D_res {
    int32 a;
};

struct D_struct {
    D_res a;

    function uint32 fun() {
        return 10;
    }

    function D_res fun2() {
        return a;
    }
};

/**
 * E
 */
struct E_struct {
    string a;
};

/**
 * F
 */
struct F_child {
    int32 a;
};

struct F_parent {
    F_child a;

    function int32 child_value() {
        return a.a;
    }
};

/**
 * G
 */
struct G_struct {
    string a[];
};

/** H **/

struct H_struct_a {
    string a;
};

struct H_struct_b {
    string a;
};

/** I **/

struct I_struct_a(int32 a) {};

struct I_struct_b {
    int32 a;
    I_struct_a(a) b;
};

/** J **/

struct J_struct {
    int32 a;
    int32 b if a > 0;
};

/** K **/

struct K_struct_a {
    int32 a;
};

struct K_struct_b {
    K_struct_a a[];
};

/** L - Lookup member type declared after parent type */

struct L_parent {
    L_member a;
};

struct L_member {
    int32 a;
};


/** M - Check field type information */

struct M_child {
    int32 a;
};

struct M_parent {
    int32 a;
    uint16 b;
    float32 c;
    string d;
    M_child e;
};

/** N - Set incompatible instance to field */
struct N_expected {
    int32 a;
};

struct N_wrong {
    int32 a;
};

struct N_parent {
    N_expected a;
};

/** O - Array field type */
struct O_parent {
    string a[3];
};
