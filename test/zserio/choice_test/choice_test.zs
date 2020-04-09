package choice_test;

choice choice_a(int32 s) on s {
case 0:
    int32 a;
case 1:
    float32 b;
default:
    string c;
};
