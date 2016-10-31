// 这个例子用哲学家就餐问题来说明rust的并发编程


/*
问题描述：
在远古时代，一个富有的慈善家捐赠了一个学院来为 5 名知名的哲学家提供住处。每个哲学家都有一个房间来进行他专业的思考活动；这也有一个共用的餐厅，布置了一个圆桌，周围放着 5 把椅子，每一把都标出了坐在这的哲学家的名字。哲学家们按逆时针顺序围绕桌子做下。每个哲学家的左手边放着一个金叉子，而在桌子中间有一大碗意大利面，它会不时的被补充。哲学家期望用他大部分的时间思考；不过当他饿了的时候，他走向餐厅，坐在它自己的椅子上，拿起他左手边自己的叉子，然后把它插进意大利面。不过乱成一团的意大利面需要第二把叉子才能吃到嘴里。因此哲学家不得不拿起他右手边的叉子。当他吃完了他会放下两把叉子，从椅子上起来，并继续思考。当然，一把叉子一次同时只能被一名哲学家使用。如果其他哲学家需要它，他必须等待直到叉子再次可用。
 */


// 然后下面就针对上面的这个问题进行建模和求解：


// 使用多线程库和掩饰函数
use std::thread;
use std::time::Duration;

// 模拟叉子，使用多线程的互斥锁来管理竞争资源
use std::sync::{Mutex, Arc};
struct Table {
    //这个Table有一个Mutex的vector，一个互斥锁是一个控制并发的方法：一次只有一个线程能访问它的内容。
    //我们用了一个空元组，()，在互斥锁的内部，因为我们实际上并不准备使用这个值，只是要持有它。
    forks: Vec<Mutex<()>>,
}

// 首先建立一个哲学家模型
struct Philosopher {
    name: String,//目前暂时只需要一个名字来代表不同的哲学家

    //对哲学家添加左右的叉子，用usize来表示它们，因为它是你的 vector 的索引的类型。
    //这两个值将会是我们Table中的forks的索引。
    left: usize,
    right:usize,
}

// 然后使用impl在Philosopher上面定义方法，因为通常来说，处理一个拥有它自己数据的类型要比使用引用的数据来的简单。这就是一个类的模型。
impl Philosopher {
    // 创建了一个名称为new的关联函数，这个函数接受一个name，&str类型的参数（这是另一个字符串的引用），然后返回了一个我们Philosopher结构体的实例。
    fn new(name: &str, left: usize, right: usize) -> Philosopher {
        Philosopher {
            name: name.to_string(),
            left: left,
            right: right,//添加初始化函数的参数设置
        }
    }
    //Rust是一个“基于表达式”的语言，它意味着Rust中几乎所有的东西都是一个表达式并返回一个值。所以这儿没有显示的返回，函数的最后表达式是自动返回的。

    // 添加一个打印当前哲学家名字的方法，因为他显示的获取了self参数：可以直接是self参数，也可以是self的引用。
    // 并且指定输出为String类型，需要显式的返回，或者返回最后一个表达式
    fn showName(self) -> String {
        println!("{}",self.name);
        //return self.name;
        self.name//在表达式结尾加上分号;可忽略掉该值，所以这儿需要将函数体的最后一个表达式的值作为整个函数的值返回。但是这样写还是不如上面的明确。
    }

    /*
    // 添加了让哲学家表示正在吃饭的方法
    fn eat(&self) {
    println!("{} is done eating.", self.name);
}
     */

    /*
    // 实际的吃饭是需要一个过程的，所以修改如下L：
    fn eat(&self){
    println!("{} is eating.", self.name);
    // 使用多线程同并发吃法，哈哈哈
    thread::sleep(Duration::from_millis(1000));
    println!("{} is done eating.", self.name);
}
     */

    // 添加使用左右两个叉子吃饭的过程
    //访问Table的叉子列表，接着使用self.left和self.right来访问特定索引位置的叉子。这让我们访问索引位置的Mutex，并且我们在其上调用lock()。如果互斥锁目前正在被别人访问，我们将阻塞直到它可用为止。
    fn eat(&self, table: &Table){
        
        // 使用下划线表示不打算使用这些值，仅仅是获取而已，这样rust就不会警告错误了
        let _left = table.forks[self.left].lock().unwrap();
        //我们也在第一把叉子被拿起来和第二把叉子拿起来之间调用了一个thread::sleep，因为拿起叉子的过程并不是立即完成的。
        thread::sleep(Duration::from_millis(150));
        let _right = table.forks[self.right].lock().unwrap();

        //然后使用叉子吃
        println!("{} is eating.", self.name);
        thread::sleep(Duration::from_millis(1000));
        println!("{} is done eating.", self.name);

        //最后结尾处并没有显式的对叉子的释放，这会在_left和_right离开作用域时发生，自动的。
    }
}


fn main_test1() {
    /*
    // 尝试建立一个哲学家的实例
    let p1 = Philosopher::new("Judith Butler");
    let df = p1.showName();
    println!("{} success",df);
    */

    /*
    // 现在批量生产哲学家
    //创建了一个Vec<T>。Vec<T>也叫做一个“vector”，它是一个可增长的数组类型。
    let philosophers = vec![
        Philosopher::new("Judith Butler"),
        Philosopher::new("Gilles Deleuze"),
        Philosopher::new("Karl Marx"),
        Philosopher::new("Emma Goldman"),
        Philosopher::new("Michel Foucault"),
        ];
     */
    
    /*
    //接着我们用for循环遍历 vector，顺序获取每个哲学家的引用。
    for p in &philosophers {
        // 然后调用每个哲学家的吃饭情况：
        p.eat();
    }*/
    // 上述循环其实没有做到并发，因为是按照容器中的顺序进行遍历的
    //(1)首先我们创建一些线程，然后绑定到handles上，并且它们会返回一些这些线程句柄来让我们控制它们的行为。
    //_是一个类型占位符。我们是在说“handles是一些东西的 vector，不过Rust你自己应该能发现这些东西是什么"。
    //(2)在列表并上调用into_iter()。它创建了一个迭代器来获取每个哲学家的所有权。这样就可以将每个哲学家传递给线程。
    //(3)然后在取得的这个迭代器上调用map，他会获取一个闭包作为参数并按顺序在每个元素上调用这个闭包。
    /*
    let handles: Vec<_> = philosophers.into_iter().map(|p| {
        //thread::spawn获取一个闭包作为参数并在一个新线程执行这个闭包。这个闭包需要一个额外的标记，move，来表明这个闭包将会获取它获取的值的所有权。主要指map函数的p变量。
        thread::spawn(move || {
            //在线程中，所有我们做的就是在p上调用eat()
            p.eat();
        }) //thread::spawn调用最后木有分号，这使它是一个表达式。这个区别是重要的，以便生成正确的返回值。
    }).collect();//最后，我们获取所有这些map调用的结果并把它们收集起来。collect()将会把它们放入一个某种类型的集合，这也就是为什么我们要表明返回值的类型：我们需要一个Vec<T>。这些元素是thread::spawn调用的返回值，它们就是这些线程的句柄。

    // 然后遍历这个集合，从而遍历句柄并在其上调用join()，它会阻塞执行直到线程完成执行。
    for h in handles {
        h.join().unwrap();
    }
     */
    
    // 代码到这儿为止，还没有模拟叉子，添加叉子需要大量的代码修改，我们将写在另一个函数中。
}


// 第二个实现函数
fn main_test2(){
    //创建一个Table并封装在一个Arc<T>中。“arc”代表“原子引用计数”，并且我们需要在多个线程间共享我们的Table。因为我们共享了它，引用计数会增长，而当每个线程结束，它会减少。
    let table = Arc::new(Table { forks: vec![
        Mutex::new(()),
        Mutex::new(()),
        Mutex::new(()),
        Mutex::new(()),
        Mutex::new(()),
        ]});

    // 注意：在构造哲学家的时候传入了左右叉子的序号，这些序号是连续的，最后一个应该是4,0，但是这儿反过来了，表示有一个是左撇子，这是为了解决死锁。
    let philosophers = vec![
        Philosopher::new("Judith Butler", 0, 1),
        Philosopher::new("Gilles Deleuze", 1, 2),
        Philosopher::new("Karl Marx", 2, 3),
        Philosopher::new("Emma Goldman", 3, 4),
        Philosopher::new("Michel Foucault", 0, 4),
        ];

    let handles: Vec<_> = philosophers.into_iter().map(|p| {
        //Arc<T>的clone()方法用来增加引用计数，而当它离开作用域，它减少计数。
        //并且用了一个新的table来覆盖旧的table
        let table = table.clone();
        //然后启动多线程开始吃饭
        thread::spawn(move || {
            p.eat(&table);
        })
    }).collect();

    for h in handles {
        h.join().unwrap();
    }
}

// 在实际函数中调用不同的测试函数
fn main(){
    // 这样就是一个简单的并发问题的解决方式，主要体现在多线程的启动和竞争资源的管理方式上，并且使用容器来对一类实例进行管理。
    main_test2();
}
