use std::thread;

// 增加一个新的属性，no_mangle。当你创建了一个Rust库，编译器会在输出中修改函数的名称。这么做的原因超出了本教程的范围，不过为了其他语言能够知道如何调用这些函数，我们需要禁止这么做。这个属性将它关闭。这样其他程序才能用这个名字调用库中的这个函数。
#[no_mangle]
// pub意味着这个函数应当从模块外被调用，而extern说它应当能被C调用。
pub extern fn process() {
    let handles: Vec<_> = (0..10).map(|_| {
        thread::spawn(|| {
            let mut x = 0;
            for _ in 0..5_000_000 {
                x += 1
            }
            x
        })
    }).collect();

    for h in handles {
        println!("Thread finished with count={}",
        h.join().map_err(|_| "Could not join a thread!").unwrap());
    }
}
