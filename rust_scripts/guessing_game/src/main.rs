// 表示使用外部库，不然无法编译，虽然只说了我们要依赖rand，我们也获取了一份libc的拷贝。这是因为rand依赖libc工作。
extern crate rand;

// 使用标准库的io库。Rust 为所有程序只导入了很少一些东西，‘prelude’。如果它不在预先导入中，你将不得不直接use它，需要自己手动明确的指定需要的库。
use std::io;

// rust使用外部库来支持random的生成，而不是语言内建，所以需要使用cargo来对这些基本库的版本进行管理。然后可以和内部支持库一样使用rand来指明在rand作用域中才能工作。
use rand::Rng;


// 引入比较类型到作用域中
use std::cmp::Ordering;


// fn 表示这个是一个函数，函数的三个要素：函数名，参数列表和返回值类型
// main函数在Rust中也是非常特殊的，是一切程序的入口，和C/C++相同，main函数没有参数，所以使用空的括号体，然后在大括号中表示当前的程序编写。
fn main(){
    // println!表示这是一个宏，而不是一个函数，如果是函数就没有符号!
    // 宏是rust的元编程机制，后续再继续了解
    println!("Hello{}","world");// 使用分号表示一行的结束，这个是和C/C++相同的。

    // （2）然后需要生成一个范围在0-100之间的随机数
    //其中：rand::thread_rng()函数来获取一个随机数生成器的拷贝，它位于我们特定的执行线程的本地。这个函数包含指定的下限，但是不包含指定的上限。
    let secret_number = rand::thread_rng().gen_range(1, 101);
    //println!("current automatic gen code is:{}",secret_number);//打印出密码就没有乐趣了

    // （1）首先需要从键盘获取用户的输入，并且回显给用户
    println!("Guess the number!");

    // 使用循环包裹需要重复执行的代码体，然后处理退出的代码：
    loop {
        println!("Please input your guess.");
        //使用了let语句创建变量绑定，默认是不可改变的；但是指定了mut的，表示这个是可以动态绑定的变量；
        //String是一个字符串类型，由标准库提供。String是一个可增长的，UTF-8编码的文本。函数new()创建一个空的string
        let mut guess = String::new();
        //如果不在最上面使用：use std::io，那么我们就得写成std::io::stdin()，这个特殊的函数返回一个指向你终端标准输入的句柄。这儿相当于C++的命名空间引入。
        //通过句柄调用了read_line()函数，并且传递了一个参数&mut guess，并且是可变变量guess的一个引用。read_line函数只接受可变参数的引用作为参数，因为这个函数将获取用户输入并且存放在这个可变参数中。
        //read_line将会返回一个一个io::Result类型的结果（Rust的标准库中有很多叫做Result的类型：一个泛型Result，然后是子库的特殊版本，例如io::Result），io::Result有一个expect()方法获取调用它的值，用于错误处理（异常），这种内建的异常机制在java中是非常常见的。
        io::stdin().read_line(&mut guess).expect("Failed to read line");

        //（4）在这儿对guess进行类型转换，并且重新进行绑定
        //guess开始是一个String，不过我们想要把它转换为一个u32。遮盖（Shadowing）让我们重用guess名字，而不是强迫我们想出两个独特的像guess_str和guess，或者别的什么。
        //字符串的parse()方法将字符串解析为一些类型的数字。因为它可以解析多种数字，我们需要给Rust一些提醒作为我们具体想要的数字的类型。因此，let guess: u32。guess后面的分号（:）告诉 Rust 我们要标注它的类型。
        //let guess: u32 = guess.trim().parse()
        //就像read_line()，我们调用parse()可能产生一个错误。如果我们的字符串包含A👍%?呢？并不能将它们转换成一个数字。为此，我们将做我们在read_line()时做的相同的事：使用expect()方法来在这里出现错误时崩溃。
        //.expect("Please type a number!");

        // 限定一下输入一定是数字
        let guess: u32 = match guess.trim().parse(){
            Ok(num) => num,//表示成功的解析为整型，然后返回这个整型
            Err(_) => continue,//Err的情况下，我们并不关心它是什么类型的错误，所以我们仅仅使用_而不是一个名字，表示忽略了错误类型。然后就使用continue来继续下一次的loop。
        };
        // 和上面的不同，这儿没有使用expect来抛出异常，而是使用了一个程序体来处理，parse返回的Result就是一个像Ordering一样的枚举，然后交给match来进行处理
        
        // 使用宏来输出当前的用户输入内容，其中{}是一个占位符，所以我们传递guess作为一个参数。可以有多个占位符，也就是多个括号，但是也需要传递多个参数。
        println!("You guessed: {}", guess);

        // （3）接下来就需要对用户的输入和随机生成的数字进行比较了：
        //cmp函数可以作用在任何可以比较的类型的值上，然后它获取你想要比较的值的引用，返回我们之前use的Ordering类型，Ordering是一个枚举（enum）。
        //然后我们使用一个match语句来决定具体是哪种Ordering，Ordering枚举有3个可能的变量：Less，Equal和Greater，match语句获取类型的值，并让你为每个可能的值创建一个“分支”。
        match guess.cmp(&secret_number) {
            //因为有 3 种类型的Ordering，我们有 3 个分支：
            Ordering::Less    => println!("Too small!"),//如果是小于，就打印小于信息，下面类同。
            Ordering::Greater => println!("Too big!"),
            Ordering::Equal   => {
                println!("You win!");
                break;//新增break，用来猜中的时候退出
            },
        }
    }

    // 程序到此为止，基本功能完成了，但是强类型的rust无法使用cmp来比较一个string的guess和number类型的secret_number。所以还需要对上述比较之前的部分进行类型的转换，否则无法完成编译。

    // 如果没有循环，这个程序就执行一次，我们需要将程序循环起来。loop关键字给我们一个无限循环。将上面的猜测和比较用循环包含起来，并且留出猜中 以后的退出指令。
    // 但是在输入环节中，如果我们输入了无法转换为数字的输入内容，默认是包含一个break的，我们最好处理来限定输入一定是数字。
}
