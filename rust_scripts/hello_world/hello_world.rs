// 使用标准库的io库。Rust 为所有程序只导入了很少一些东西，‘prelude’。如果它不在预先导入中，你将不得不直接use它，需要自己手动明确的指定需要的库。
use std::io;

// fn 表示这个是一个函数，函数的三个要素：函数名，参数列表和返回值类型
// main函数在Rust中也是非常特殊的，是一切程序的入口，和C/C++相同，main函数没有参数，所以使用空的括号体，然后在大括号中表示当前的程序编写。
fn main(){
    // println!表示这是一个宏，而不是一个函数，如果是函数就没有符号!
    // 宏是rust的元编程机制，后续再继续了解
    println!("Hello{}","world");// 使用分号表示一行的结束，这个是和C/C++相同的。    
}
