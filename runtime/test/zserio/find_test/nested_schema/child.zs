package nested_schema.child;

struct RequestResponse {
    int32 x;
};

service X_service {
    RequestResponse serviceMethod(RequestResponse);
};
