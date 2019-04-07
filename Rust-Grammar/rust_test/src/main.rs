
struct MyStruct<'a> {
    cache : i128,
    string : &'a str,
    f_ptr : Fn() -> u8
}

union MyUnion{
    _f32 : f32,
    _u32 : u32
}

enum MyEnum{
    None,
    Field1(String, u32),
    Field2(f32)
}


fn main() {

    let a = break;
    println!("Hello, world!");
}
