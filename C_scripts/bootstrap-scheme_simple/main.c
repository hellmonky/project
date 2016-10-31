/*
* Bootstrap Scheme - a quick and very dirty Scheme interpreter.
* Copyright (C) 2010 Peter Michaux (http://peter.michaux.ca/)
*
* This program is free software: you can redistribute it and/or
* modify it under the terms of the GNU Affero General Public
* License version 3 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License version 3 for more details.
*
* You should have received a copy of the GNU Affero General Public
* License version 3 along with this program. If not, see
* <http://www.gnu.org/licenses/>.
*/


/*
由于从unix系统移植过来，基本C函数会提示不安全，VS2013编译错误C4996，解决方式为：
项目->属性->配置属性->C/C++ -> 预处理器 -> 预处理器定义，增加_CRT_SECURE_NO_DEPRECATE
编写注释一定要注意：先读懂函数本身，然后再标明这个函数的作用，输入输出等
*/

/*
在一个“联合”内可以定义多种不同的数据类型， 一个被说明为该“联合”类型的变量中，允许装入该“联合”所定义的任何一种数据，这些数据共享同一段内存，已达到节省空间的目的（还有一个节省空间的类型：位域）。
这是一个非常特殊的地方，也是联合的特征。另外，同struct一样，联合默认访问权限也是公有的，并且，也具有成员函数。
但是结构体和联合的内存分布是不同的：
在结构中各成员有各自的内存空间， 一个结构变量的总长度是各成员长度之和（空结构除外，同时不考虑边界调整）。
而在“联合”中，各成员共享一段内存空间， 一个联合变量的长度等于各成员中最长的长度。
应该说明的是， 这里所谓的共享不是指把多个成员同时装入一个联合变量内， 而是指该联合变量可被赋予任一成员值，但每次只能赋一种值， 赋入新值则冲去旧值。
*/

/*
make_symbol：define构造符号。
函数与整数或字符串一样，也属于数据类型的一种。它有自己的字面表示形式（literal representation），能够储存在变量中，也能当作参数传递。一种数据类型应该有的功能，它都有。
符号（symbol）类型。符号实际上是一种指针，指向储存在哈希表中的字符串。所以，比较两个符号是否相等，只要看它们的指针是否一样就行了，不用逐个字符地比较。
总而言之：所有的东西都是一个对象，这就是scheme（lisp）的类型系统。
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/**************************** MODEL ******************************/
//以下内容是基本模型建立过程

//定义对象的基本类型：使用枚举类型，因为变量的取值被限定在一个有限的范围内。默认从0开始
typedef enum {
	THE_EMPTY_LIST, BOOLEAN, SYMBOL, FIXNUM,
	CHARACTER, STRING, PAIR, PRIMITIVE_PROC,
	COMPOUND_PROC, INPUT_PORT, OUTPUT_PORT,
	EOF_OBJECT
} object_type;

//对象结构体：核心数据结构，过程操作的唯一类型，系统内置的和用户自定义的没有区别。
//这儿就表明隐式类型，因为没有变量的类型，只有对象的类型，也就是说类型和对象关联。
//不存在有类型的变量的概念，一切都是基本对象的实例。
typedef struct object {
	object_type type;
	union {
		struct {
			char value;
		} boolean;
		struct {
			char *value;
		} symbol;
		struct {
			long value;
		} fixnum;
		struct {
			char value;
		} character;
		struct {
			char *value;
		} string;
		struct {
			struct object *car;
			struct object *cdr;
		} pair;//序对：使用双指针，类似于链表，lisp本身就是为了处理链表
		struct {
			struct object *(*fn)(struct object *arguments);
		} primitive_proc;//基本过程：返回为对象类型的函数指针
		struct {
			struct object *parameters;
			struct object *body;
			struct object *env;
		} compound_proc;//符合过程：使用参数列表，过程体指针（函数指针）和环境指针
		struct {
			FILE *stream;
		} input_port;//输入文件：文件流指针
		struct {
			FILE *stream;
		} output_port;//输出文件：文件流指针
	} data;
} object;//外部使用结构体设置该对象的类型和值，内部使用联合体来容纳不同的值类型实例

//没有加入GC机制，所以需要一直申请内存来保证运行。GC机制的加入比较复杂，文章中给出了相关讨论。
/* no GC so truely "unlimited extent" */
object *alloc_object(void) {
	object *obj;

	obj = malloc(sizeof(object));//每次都申请一个对象大小的内存，没有使用任何内存管理方面的技术
	if (obj == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}
	return obj;
}

//声明基本变量
object *the_empty_list;
object *false;
object *true;
object *symbol_table;
object *quote_symbol;
object *define_symbol;
object *set_symbol;
object *ok_symbol;
object *if_symbol;
object *lambda_symbol;
object *begin_symbol;
object *cond_symbol;
object *else_symbol;
object *let_symbol;
object *and_symbol;
object *or_symbol;
object *eof_object;
object *the_empty_environment;
object *the_global_environment;
//声明基本常用过程
object *cons(object *car, object *cdr);
object *car(object *pair);
object *cdr(object *pair);

//用关系运算符“==”来检测是否相同，返回char类型结果，因为C89没有内建的bool类型？
char is_the_empty_list(object *obj) {
	return obj == the_empty_list;
}

char is_boolean(object *obj) {
	return obj->type == BOOLEAN;
}

char is_false(object *obj) {
	return obj == false;
}

char is_true(object *obj) {
	return !is_false(obj);
}

//符号构造函数：对于输入字符串指定的符号类型，通过检查symbol_table中否有对应的类型来构造这种类型的对象。，然后返回这个类型。
//963行用于构造基本过程
object *make_symbol(char *value) {
	object *obj;
	object *element;

	//首先获取当前的类型定义表，symbol_table是一个全局维护量，用于说明当前系统中存在的类型
	element = symbol_table;

	/* search for they symbol in the symbol table */
	//如果当前的对象类型表symbol_table不为空，也就是说当前已经存在类型定义，那么就查找这个类型定义中已有的类型是否和value指定类型相同。
	while (!is_the_empty_list(element)) {
		//如果相同就返回已有类型
		if (strcmp(car(element)->data.symbol.value, value) == 0) {
			return car(element);//返回匹配的类型（也是一个对象）
		}
		//如果不同就使用cdr获取symbol_table表中后续的类型，继续检查
		element = cdr(element);
	};

	/* create the symbol and add it to the symbol table */
	//如果当前的对象类型表symbol_table为空，表示当前没有类型定义，那么就将这个类型加入到symbol_table表中。
	obj = alloc_object();
	obj->type = SYMBOL;
	obj->data.symbol.value = malloc(strlen(value) + 1);//为新增的类型申请空间
	if (obj->data.symbol.value == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}
	strcpy(obj->data.symbol.value, value);//复制新增类型到这个类型的专有描述obj中（因为一切都是object，从字符串构造出一个object）
	symbol_table = cons(obj, symbol_table);//然后将新增的类型添加到已有的symbol_table中
	return obj;//返回当前类型对象
}

//检测当前对象是否为一个symbol
char is_symbol(object *obj) {
	return obj->type == SYMBOL;//对象的类型是否是SYMBOL，枚举只能有一个当前值
}

//构造fixnum对象：系统提供的基本类型，所以不需要检查当前symbol_table中是否有对应类型
object *make_fixnum(long value) {
	object *obj;

	obj = alloc_object();
	obj->type = FIXNUM;//直接指定类型
	obj->data.fixnum.value = value;
	return obj;//最后返回的还是一个object，所以用户自定义类型和系统提供的类型没有区别
}

char is_fixnum(object *obj) {
	return obj->type == FIXNUM;
}

object *make_character(char value) {
	object *obj;

	obj = alloc_object();
	obj->type = CHARACTER;
	obj->data.character.value = value;
	return obj;
}

char is_character(object *obj) {
	return obj->type == CHARACTER;
}

object *make_string(char *value) {
	object *obj;

	obj = alloc_object();
	obj->type = STRING;
	obj->data.string.value = malloc(strlen(value) + 1);//需要申请空间来放置，多申请一个是为了存放结束符
	if (obj->data.string.value == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}
	strcpy(obj->data.string.value, value);
	return obj;
}

char is_string(object *obj) {
	return obj->type == STRING;
}

//基本过程：构造序对，通过指针来合成一个结构
object *cons(object *car, object *cdr) {
	object *obj;

	obj = alloc_object();
	obj->type = PAIR;//默认构造了一个序对
	obj->data.pair.car = car;
	obj->data.pair.cdr = cdr;
	return obj;
}

char is_pair(object *obj) {
	return obj->type == PAIR;
}

//获取序对的第一个元素，他默认传入的是序对结构，获取data结构中的第一个指针
object *car(object *pair) {
	return pair->data.pair.car;
}

void set_car(object *obj, object* value) {
	obj->data.pair.car = value;
}

//获取序对的最后一个元素，他默认传入的是序对结构，获取data结构中的末尾指针
object *cdr(object *pair) {
	return pair->data.pair.cdr;
}

void set_cdr(object *obj, object* value) {
	obj->data.pair.cdr = value;
}

//这儿可以用字符串匹配来自动构造，不过一般来讲也很少直接使用嵌套结果过深的car和cdr
#define caar(obj)   car(car(obj))
#define cadr(obj)   car(cdr(obj))
#define cdar(obj)   cdr(car(obj))
#define cddr(obj)   cdr(cdr(obj))
#define caaar(obj)  car(car(car(obj)))
#define caadr(obj)  car(car(cdr(obj)))
#define cadar(obj)  car(cdr(car(obj)))
#define caddr(obj)  car(cdr(cdr(obj)))
#define cdaar(obj)  cdr(car(car(obj)))
#define cdadr(obj)  cdr(car(cdr(obj)))
#define cddar(obj)  cdr(cdr(car(obj)))
#define cdddr(obj)  cdr(cdr(cdr(obj)))
#define caaaar(obj) car(car(car(car(obj))))
#define caaadr(obj) car(car(car(cdr(obj))))
#define caadar(obj) car(car(cdr(car(obj))))
#define caaddr(obj) car(car(cdr(cdr(obj))))
#define cadaar(obj) car(cdr(car(car(obj))))
#define cadadr(obj) car(cdr(car(cdr(obj))))
#define caddar(obj) car(cdr(cdr(car(obj))))
#define cadddr(obj) car(cdr(cdr(cdr(obj))))
#define cdaaar(obj) cdr(car(car(car(obj))))
#define cdaadr(obj) cdr(car(car(cdr(obj))))
#define cdadar(obj) cdr(car(cdr(car(obj))))
#define cdaddr(obj) cdr(car(cdr(cdr(obj))))
#define cddaar(obj) cdr(cdr(car(car(obj))))
#define cddadr(obj) cdr(cdr(car(cdr(obj))))
#define cdddar(obj) cdr(cdr(cdr(car(obj))))
#define cddddr(obj) cdr(cdr(cdr(cdr(obj))))

//构造基本过程：通过传入一个指向过程的指针构造基本过程，返回带有过程类型的object对象
object *make_primitive_proc( object *(*fn)(struct object *arguments) ) {
	object *obj;

	obj = alloc_object();
	obj->type = PRIMITIVE_PROC;
	obj->data.primitive_proc.fn = fn;
	return obj;
}

char is_primitive_proc(object *obj) {
	return obj->type == PRIMITIVE_PROC;
}

/*
使用car和cdr来获取内容判断，也就是说这里将proc当做一个序对！
序对由两个object构成，第一个是过程体，第二个是过程的参数。
*/

//通过检查输入object的第一个元素是否为空来确定是否为空过程（这个判断有些特殊）
object *is_null_proc(object *arguments) {
	return is_the_empty_list(car(arguments)) ? true : false;
}

//通过检查输入object的第一个元素的类型来判断过程是否是已有类型
object *is_boolean_proc(object *arguments) {
	return is_boolean(car(arguments)) ? true : false;
}

object *is_symbol_proc(object *arguments) {
	return is_symbol(car(arguments)) ? true : false;
}

object *is_integer_proc(object *arguments) {
	return is_fixnum(car(arguments)) ? true : false;
}

object *is_char_proc(object *arguments) {
	return is_character(car(arguments)) ? true : false;
}

object *is_string_proc(object *arguments) {
	return is_string(car(arguments)) ? true : false;
}

object *is_pair_proc(object *arguments) {
	return is_pair(car(arguments)) ? true : false;
}

//通过类型标签检查是否为复合过程
char is_compound_proc(object *obj);

//检查输入object是否是一个过程，包含基本过程primitive_proc和复合过程compound_proc
object *is_procedure_proc(object *arguments) {
	object *obj;

	obj = car(arguments);//默认是序对，获取object的第一个对象
	return (is_primitive_proc(obj) ||
		is_compound_proc(obj)) ?
		true :
		false;
}

//类型转换：就是提取出当前object的值，然后放在对应类型的值域中，都是指针操作，相当于强制类型转换
object *char_to_integer_proc(object *arguments) {
	return make_fixnum((car(arguments))->data.character.value);
}

object *integer_to_char_proc(object *arguments) {
	return make_character((car(arguments))->data.fixnum.value);
}

object *number_to_string_proc(object *arguments) {
	char buffer[100];

	sprintf(buffer, "%ld", (car(arguments))->data.fixnum.value);
	return make_string(buffer);
}

object *string_to_number_proc(object *arguments) {
	return make_fixnum(atoi((car(arguments))->data.string.value));
}

object *symbol_to_string_proc(object *arguments) {
	return make_string((car(arguments))->data.symbol.value);
}

object *string_to_symbol_proc(object *arguments) {
	return make_symbol((car(arguments))->data.string.value);
}

/*
一下过程涉及到对list的遍历，应该抽象为map过程，两个参数：要遍历的过程和要执行的操作
*/

//基本过程的定义：对一个list中每个成员的值累加并且返回数据类型object
object *add_proc(object *arguments) {
	long result = 0;

	while (!is_the_empty_list(arguments)) {
		result += (car(arguments))->data.fixnum.value;
		arguments = cdr(arguments);
	}
	return make_fixnum(result);
}

object *sub_proc(object *arguments) {
	long result;

	result = (car(arguments))->data.fixnum.value;
	while (!is_the_empty_list(arguments = cdr(arguments))) {
		result -= (car(arguments))->data.fixnum.value;
	}
	return make_fixnum(result);
}

object *mul_proc(object *arguments) {
	long result = 1;

	while (!is_the_empty_list(arguments)) {
		result *= (car(arguments))->data.fixnum.value;
		arguments = cdr(arguments);
	}
	return make_fixnum(result);
}

//求过程中第一个object的商数。不理解这个过程的目的是什么
object *quotient_proc(object *arguments) {
	return make_fixnum(
		((car(arguments))->data.fixnum.value) /
		((cadr(arguments))->data.fixnum.value));
}

object *remainder_proc(object *arguments) {
	return make_fixnum(
		((car(arguments))->data.fixnum.value) %
		((cadr(arguments))->data.fixnum.value));
}

//判断当前object中所有元素值是否都相同，也可以利用map来改写，但不方便
object *is_number_equal_proc(object *arguments) {
	long value;

	value = (car(arguments))->data.fixnum.value;//获取当前传入object的值
	while (!is_the_empty_list(arguments = cdr(arguments))) {//首先将当前object的后继赋值给object本身，并且检查后继是否为空，如果不为空进入循环
		if (value != ((car(arguments))->data.fixnum.value)) {//获取当前的object值与第一次传入的object值进行比较，如果不同就返回，否则继续比较
			return false;
		}
	}
	return true;
	/*如果自己写：
	long value;
	value = (car(arguments))->data.fixnum.value;
	object *tmp = arguments;
	while (!is_the_empty_list(tmp)) {
		if (value != (tmp)->data.fixnum.value)) {
			return false;
		}
		tmp = cdr(arguments);
	}
	return true;
	*/
}

//判断一个list是不是由从小到大顺序值排列
object *is_less_than_proc(object *arguments) {
	long previous;
	long next;

	previous = (car(arguments))->data.fixnum.value;//获取当前object的值
	while (!is_the_empty_list(arguments = cdr(arguments))) {
		next = (car(arguments))->data.fixnum.value;//获取剩余list中第一个object的值
		if (previous < next) {//如果前驱小于后继，将后继值赋值给前驱，然后继续判断后续
			previous = next;
		}
		else {//如果前驱大于后继就返回错误
			return false;
		}
	}
	return true;
}

object *is_greater_than_proc(object *arguments) {
	long previous;
	long next;

	previous = (car(arguments))->data.fixnum.value;
	while (!is_the_empty_list(arguments = cdr(arguments))) {
		next = (car(arguments))->data.fixnum.value;
		if (previous > next) {
			previous = next;
		}
		else {
			return false;
		}
	}
	return true;
}

//将一个过程构建为序对：这个过程的目的是什么？
object *cons_proc(object *arguments) {
	return cons(car(arguments), cadr(arguments));
}

//获取过程的第一个object参数
object *car_proc(object *arguments) {
	return caar(arguments);
}

//获取过程的除第一个后续object参数
object *cdr_proc(object *arguments) {
	return cdar(arguments);
}

//将过程object的后续object对象作为值赋值给第一个object的数据区pair的第一个参数。重要过程：可以看做过程参数传入
object *set_car_proc(object *arguments) {
	set_car(car(arguments), cadr(arguments));
	return ok_symbol;
}

//将过程object的后续object对象作为值赋值给第一个object的数据区pair的第二个参数。
object *set_cdr_proc(object *arguments) {
	set_cdr(car(arguments), cadr(arguments));
	return ok_symbol;
}

object *list_proc(object *arguments) {
	return arguments;
}

//默认proc是序对，比较序对两个元素类型是否相同
object *is_eq_proc(object *arguments) {
	object *obj1;
	object *obj2;

	obj1 = car(arguments);
	obj2 = cadr(arguments);

	//比较序对第一个和后续元素的类型是否相同
	if (obj1->type != obj2->type) {
		return false;
	}
	//根据值的类型，对值的相等做出判断，然后返回是否相等（这样做是因为使用了联合，所以要对每一种情况都作出处理）
	switch (obj1->type) {
	case FIXNUM:
		return (obj1->data.fixnum.value ==
			obj2->data.fixnum.value) ?
			true : false;
		break;
	case CHARACTER:
		return (obj1->data.character.value ==
			obj2->data.character.value) ?
			true : false;
		break;
	case STRING:
		return (strcmp(obj1->data.string.value,
			obj2->data.string.value) == 0) ?
			true : false;
		break;
	default:
		return (obj1 == obj2) ? true : false;
	}
}

//基本过程不能运行。作为其他过程的检测过程被调用，调用的方式是对比两个函数指针是否相同，应该还有一个求值错误的判断和赋值
object *apply_proc(object *arguments) {
	fprintf(stderr, "illegal state: The body of the apply "
		"primitive procedure should not execute.\n");
	exit(1);
}

//返回当前交互环境：环境是scheme中非常重要的概念
object *interaction_environment_proc(object *arguments) {
	return the_global_environment;//返回当前全局环境
}

//初始化一个空环境，用于全局交互。这儿是一个声明，用于下面的调用。（感觉写的比价仓促，整个流程还是可以改善的）
object *setup_environment(void);

//明确指定初始化一个空环境
object *null_environment_proc(object *arguments) {
	return setup_environment();
}

//构造一个环境：用于子过程eval时的环境设置
object *make_environment(void);

//通过传入参数构造环境，还是构造了一个新的空环境（这个和设计思路有关了）
object *environment_proc(object *arguments) {
	return make_environment();
}

//基本过程不能求值。作为其他过程的检测过程被调用，调用的方式是对比两个函数指针是否相同，应该还有一个求值错误的判断和赋值
object *eval_proc(object *arguments) {
	fprintf(stderr, "illegal state: The body of the eval "
		"primitive procedure should not execute.\n");
	exit(1);
}

//文件操作：将输入文件读入，读入成功后转换为object返回
object *read(FILE *in);
//求值过程：用表达式和环境变量求值，核心过程
object *eval(object *exp, object *env);

//载入过程：文件操作的细化过程，用于操作文件中定义的过程
object *load_proc(object *arguments) {
	char *filename;
	FILE *in;
	object *exp;
	object *result = NULL;

	filename = car(arguments)->data.string.value;//传入object中的值作为要载入的文件名参数
	in = fopen(filename, "r");//如果顺利打开文件就返回指向该流的文件指针
	if (in == NULL) {
		fprintf(stderr, "could not load file \"%s\"", filename);
		exit(1);
	}
	while ((exp = read(in)) != NULL) {//用文件指针读入文件流，如果不为空（没有结束），就读一句，执行一句
		result = eval(exp, the_global_environment);//使用外部环境变量
	}
	fclose(in);//关闭文件
	return result;
}

//通过文件构造输入文件类型的object（系统支持的基本类型）
object *make_input_port(FILE *in);

//打开输入的文件类型object
object *open_input_port_proc(object *arguments) {
	char *filename;
	FILE *in;

	filename = car(arguments)->data.string.value;
	in = fopen(filename, "r");
	if (in == NULL) {
		fprintf(stderr, "could not open file \"%s\"\n", filename);
		exit(1);
	}
	return make_input_port(in);
}

//关闭输入文件类型的object
object *close_input_port_proc(object *arguments) {
	int result;

	result = fclose(car(arguments)->data.input_port.stream);
	if (result == EOF) {
		fprintf(stderr, "could not close input port\n");
		exit(1);
	}
	return ok_symbol;
}

//判断object是否为输入文件类型
char is_input_port(object *obj);
object *is_input_port_proc(object *arguments) {
	return is_input_port(car(arguments)) ? true : false;
}

//从输入object读入过程
object *read_proc(object *arguments) {
	FILE *in;
	object *result;

	//判断输入object是否为空，如果为空就从stdin读入（标准C输入函数，用于从键盘获取输入流），否则就从文件读入
	in = is_the_empty_list(arguments) ?
	stdin :
		  car(arguments)->data.input_port.stream;//输入object不为空时，将object第一个参数指定的文件赋值给in
	result = read(in);
	return (result == NULL) ? eof_object : result;
}

//从输入object读入过程，然后转换为character类型object输出
object *read_char_proc(object *arguments) {
	FILE *in;
	int result;

	in = is_the_empty_list(arguments) ?
	stdin :
		  car(arguments)->data.input_port.stream;
	result = getc(in);
	return (result == EOF) ? eof_object : make_character(result);
}

int peek(FILE *in);
//获取当前输入object指定文件或者当前键盘输入的流，然后转换为character类型object输出
object *peek_char_proc(object *arguments) {
	FILE *in;
	int result;

	in = is_the_empty_list(arguments) ?
	stdin :
		  car(arguments)->data.input_port.stream;
	result = peek(in);
	return (result == EOF) ? eof_object : make_character(result);//需要非空检测来构造
}

//检查输入object是否结束：用于判断当前从键盘或者文件读入的过程是否结束
char is_eof_object(object *obj);

object *is_eof_object_proc(object *arguments) {
	return is_eof_object(car(arguments)) ? true : false;
}

//通过熟人文件，构造写入流object
object *make_output_port(FILE *in);

object *open_output_port_proc(object *arguments) {
	char *filename;
	FILE *out;

	filename = car(arguments)->data.string.value;
	out = fopen(filename, "w");//写模式
	if (out == NULL) {
		fprintf(stderr, "could not open file \"%s\"\n", filename);
		exit(1);
	}
	return make_output_port(out);
}

object *close_output_port_proc(object *arguments) {
	int result;

	result = fclose(car(arguments)->data.output_port.stream);//关闭object中指定的文件
	if (result == EOF) {
		fprintf(stderr, "could not close output port\n");
		exit(1);
	}
	return ok_symbol;
}

//检查object是否为输入文件类型
char is_output_port(object *obj);

object *is_output_port_proc(object *arguments) {
	return is_output_port(car(arguments)) ? true : false;
}

//
object *write_char_proc(object *arguments) {
	object *character;
	FILE *out;

	character = car(arguments);//将第一个参数赋值给character
	arguments = cdr(arguments);//后继参数覆盖本身
	out = is_the_empty_list(arguments) ?//如果没有后继参数，就从键盘获取输入，否则从后继参数获取输出文件
	stdout :
		   car(arguments)->data.output_port.stream;
	putc(character->data.character.value, out);//将第一个参数写入到输出文件中
	fflush(out);//非标准函数，用于清除读写缓冲区，需要立即把输出缓冲区的数据进行物理写入时使用
	return ok_symbol;
}

//将object中的数据转义（转换为人可以识别的符号）写入到文件中去，也就是将解析过的object写到文件中，也用于交互中的显示
void write(FILE *out, object *obj);

//将传入的object解析然后写入到交互窗口或者文件中（stdout）
object *write_proc(object *arguments) {
	object *exp;
	FILE *out;

	exp = car(arguments);
	arguments = cdr(arguments);
	out = is_the_empty_list(arguments) ?
	stdout :
		   car(arguments)->data.output_port.stream;
	write(out, exp);
	fflush(out);
	return ok_symbol;
}

//错误输出，将报错的object参数按照错误输出，这个是内部解析错误与外部显示的交互过程，用于将内部判断错误的过程显示给用户
object *error_proc(object *arguments) {
	while (!is_the_empty_list(arguments)) {
		write(stderr, car(arguments));
		fprintf(stderr, " ");
		arguments = cdr(arguments);
	};
	printf("\nexiting\n");
	exit(1);
}

//构造复合过程：parameters参数列表，body过程体指针，env环境指针
object *make_compound_proc(object *parameters, object *body, object* env) {
	object *obj;

	obj = alloc_object();//申请一个空的object，然后填充参数
	obj->type = COMPOUND_PROC;
	obj->data.compound_proc.parameters = parameters;
	obj->data.compound_proc.body = body;
	obj->data.compound_proc.env = env;
	return obj;//过程也是对象，可以被返回，所以数据即代码，代码即数据
}

char is_compound_proc(object *obj) {
	return obj->type == COMPOUND_PROC;
}

object *make_input_port(FILE *stream) {
	object *obj;

	obj = alloc_object();
	obj->type = INPUT_PORT;
	obj->data.input_port.stream = stream;
	return obj;
}

char is_input_port(object *obj) {
	return obj->type == INPUT_PORT;
}

object *make_output_port(FILE *stream) {
	object *obj;

	obj = alloc_object();
	obj->type = OUTPUT_PORT;
	obj->data.output_port.stream = stream;
	return obj;
}

char is_output_port(object *obj) {
	return obj->type == OUTPUT_PORT;
}

char is_eof_object(object *obj) {
	return obj == eof_object;
}

//返回环境的后继，类似于闭包？？？
object *enclosing_environment(object *env) {
	return cdr(env);
}

/*
一个环境就是框架的一个序列，每个框架是包含着一些约束的一个表格（可以为空）；
这些约束将一些变量的名字关联到对应的值上，一个框架中，任何变量最多只能有一个约束，也就是说一个框架中变量的值是唯一的；
每个框架还包含着一个指向这个框架外围环境的指针。

综合起来：
（1）在求值的环境模型下，环境就是框架的集合；
（2）框架是一组key-value，用来说明变量和其值的对应关系；
（3）框架之间存在层次关系，并且通过指针关联，这种层次关系说明了变量的作用域；
（3）在环境中查找变量的值需要依层次关系从框架中查找，称为深约束，效率很低，是语法作用域的前身。
*/

//获取环境的第一个参数，也就是最顶层frame
object *first_frame(object *env) {
	return car(env);
}

//一个frame就是一个序对，表示为key-value关系，类似于map，用于变量和值的对应关系
object *make_frame(object *variables, object *values) {
	return cons(variables, values);
}

//获取frame的key
object *frame_variables(object *frame) {
	return car(frame);
}

//获取frame的value
object *frame_values(object *frame) {
	return cdr(frame);
}

//通过设置序对的方式将key-value绑定到frame上：也就是添加一个变量的定义到frame中
void add_binding_to_frame(object *var, object *val, object *frame) {
	set_car(frame, cons(var, car(frame)));
	set_cdr(frame, cons(val, cdr(frame)));
}

//扩展环境：添加一个新的frame到原有的env中去，关键在于新的frame在env的顶层
object *extend_environment(object *vars, object *vals, object *base_env) {
	return cons(make_frame(vars, vals), base_env);
}

//深度遍历查找变量的值：求值的环境模型，查询变量时需要按照frame层次逐步查询
object *lookup_variable_value(object *var, object *env) {
	object *frame;
	object *vars;
	object *vals;
	while (!is_the_empty_list(env)) {
		frame = first_frame(env);
		vars = frame_variables(frame);//获取当前顶层frame的所有变量列表
		vals = frame_values(frame);//获取当前顶层frame的所有变量对应值的列表
		while (!is_the_empty_list(vars)) {//当前frame不为空时开始查找
			if (var == car(vars)) {//从第一个元素开始查找
				return car(vals);
			}
			vars = cdr(vars);//如果没有，依次从后续元素查找
			vals = cdr(vals);
		}
		env = enclosing_environment(env);//当前frame没有，从下一层frame查找
	}
	fprintf(stderr, "unbound variable, %s\n", var->data.symbol.value);//在指定环境中无法找到变量定义
	exit(1);
}

//在指定环境中设置变量的值：先深度遍历，在找到该变量的第一个frame中设置值
//注意：环境应该是上下文的，这样有些粗糙，如果更下一层frame中有同名变量，该如何取舍？
//这里的处理方式是在定义变量时带上指定的环境，
void set_variable_value(object *var, object *val, object *env) {
	object *frame;
	object *vars;
	object *vals;

	while (!is_the_empty_list(env)) {
		frame = first_frame(env);
		vars = frame_variables(frame);
		vals = frame_values(frame);
		while (!is_the_empty_list(vars)) {
			if (var == car(vars)) {
				set_car(vals, val);
				return;
			}
			vars = cdr(vars);
			vals = cdr(vals);
		}
		env = enclosing_environment(env);
	}
	fprintf(stderr, "unbound variable, %s\n", var->data.symbol.value);
	exit(1);
}

//变量的定义：使用define时应该调用的过程，指定变量名，值和上下文环境
void define_variable(object *var, object *val, object *env) {
	object *frame;
	object *vars;
	object *vals;

	//在环境的顶层frame添加变量
	frame = first_frame(env);
	vars = frame_variables(frame);
	vals = frame_values(frame);

	//如果在当前frame中有这个变量名，就重新赋值
	while (!is_the_empty_list(vars)) {
		if (var == car(vars)) {
			set_car(vals, val);
			return;
		}
		vars = cdr(vars);
		vals = cdr(vals);
	}
	//否则就添加新的变量和值对
	add_binding_to_frame(var, val, frame);
}

//设置环境：初始化一个空的环境
object *setup_environment(void) {
	object *initial_env;

	initial_env = extend_environment(
		the_empty_list,
		the_empty_list,
		the_empty_environment);
	return initial_env;
}

//在指定环境中注册基本过程：因为lisp中所有东西都可以define，都是一个object，所以基本过程需要在初始环境中进行注册，然后使用
void populate_environment(object *env) {

//注册基本的scheme过程和对应过程名的关联：scheme_name供用户调用的scheme过程，c_name在这个文件中实现的过程
//注册方式：调用define_variable过程，将scheme_name注册为符号（也就是string），将c_name通过make_primitive_proc来构造基本过程，并且关联全局env
#define add_procedure(scheme_name, c_name)              \
    define_variable(make_symbol(scheme_name),           \
                    make_primitive_proc(c_name),        \
                    env);

	//将一些基本过程加入到环境中，然后使用symbol_table来索引
	add_procedure("null?", is_null_proc);
	add_procedure("boolean?", is_boolean_proc);
	add_procedure("symbol?", is_symbol_proc);
	add_procedure("integer?", is_integer_proc);
	add_procedure("char?", is_char_proc);
	add_procedure("string?", is_string_proc);
	add_procedure("pair?", is_pair_proc);
	add_procedure("procedure?", is_procedure_proc);

	add_procedure("char->integer", char_to_integer_proc);
	add_procedure("integer->char", integer_to_char_proc);
	add_procedure("number->string", number_to_string_proc);
	add_procedure("string->number", string_to_number_proc);
	add_procedure("symbol->string", symbol_to_string_proc);
	add_procedure("string->symbol", string_to_symbol_proc);

	add_procedure("+", add_proc);
	add_procedure("-", sub_proc);
	add_procedure("*", mul_proc);
	add_procedure("quotient", quotient_proc);
	add_procedure("remainder", remainder_proc);
	add_procedure("=", is_number_equal_proc);
	add_procedure("<", is_less_than_proc);
	add_procedure(">", is_greater_than_proc);

	add_procedure("cons", cons_proc);
	add_procedure("car", car_proc);
	add_procedure("cdr", cdr_proc);
	add_procedure("set-car!", set_car_proc);
	add_procedure("set-cdr!", set_cdr_proc);
	add_procedure("list", list_proc);

	add_procedure("eq?", is_eq_proc);

	add_procedure("apply", apply_proc);

	add_procedure("interaction-environment", interaction_environment_proc);
	add_procedure("null-environment", null_environment_proc);
	add_procedure("environment", environment_proc);
	add_procedure("eval", eval_proc);

	add_procedure("load", load_proc);
	add_procedure("open-input-port", open_input_port_proc);
	add_procedure("close-input-port", close_input_port_proc);
	add_procedure("input-port?", is_input_port_proc);
	add_procedure("read", read_proc);
	add_procedure("read-char", read_char_proc);
	add_procedure("peek-char", peek_char_proc);
	add_procedure("eof-object?", is_eof_object_proc);
	add_procedure("open-output-port", open_output_port_proc);
	add_procedure("close-output-port", close_output_port_proc);
	add_procedure("output-port?", is_output_port_proc);
	add_procedure("write-char", write_char_proc);
	add_procedure("write", write_proc);

	add_procedure("error", error_proc);
}

//构建环境：包含一个空的初始化环境和基本过程，这样就可以再这个环境中使用提供的功能
object *make_environment(void) {
	object *env;

	env = setup_environment();
	populate_environment(env);
	return env;
}

//初始化过程函数：完成了对基本对象和基本过程的构建
void init(void) {
	//首先构造基本对象
	the_empty_list = alloc_object();
	the_empty_list->type = THE_EMPTY_LIST;

	false = alloc_object();
	false->type = BOOLEAN;
	false->data.boolean.value = 0;

	true = alloc_object();
	true->type = BOOLEAN;
	true->data.boolean.value = 1;

	symbol_table = the_empty_list;
	quote_symbol = make_symbol("quote");
	define_symbol = make_symbol("define");
	set_symbol = make_symbol("set!");
	ok_symbol = make_symbol("ok");
	if_symbol = make_symbol("if");
	lambda_symbol = make_symbol("lambda");
	begin_symbol = make_symbol("begin");
	cond_symbol = make_symbol("cond");
	else_symbol = make_symbol("else");
	let_symbol = make_symbol("let");
	and_symbol = make_symbol("and");
	or_symbol = make_symbol("or");

	eof_object = alloc_object();
	eof_object->type = EOF_OBJECT;

	//然后构造全局环境
	the_empty_environment = the_empty_list;

	//最后构建一个包含有基本过程的全局环境
	the_global_environment = make_environment();
}

/***************************** READ ******************************/
//以下内容是文本读入分析部分，类似于编译原理中的词法分析器（参见CompilersImplementation\UCC_WIN项目）

//界定符判断：lisp中的界定符包括：空格，字符串结束符，左括号，右括号，引号，分号。除了这些界定符，其他符号将不被识别
//需要注意的是左右括号匹配问题
char is_delimiter(int c) {
	return isspace(c) || c == EOF ||
		c == '(' || c == ')' ||
		c == '"' || c == ';';
}

//判断是否是开始符号：lisp也是由语句构成的，那么就需要判断读入的字符是否是语句的开始
char is_initial(int c) {
	//isalpha函数判断字符c是否为英文字母a-z或A-Z，是就返回非零值(不一定是1)，否则返回零
	return isalpha(c) || c == '*' || c == '/' || c == '>' ||
		c == '<' || c == '=' || c == '?' || c == '!';
}

/*
ungetc是将读出的数据再次放回到缓冲区去，下一次读数据时，会再次读出来的。
ungetc()函数的作用：当你从流中读出字符，但却不是你需要的内容时，为了不改流的内容（也许此流另外有用），需要回写进流。
写回后再读，仍可以正常读出来
*/
//获取从当前文件中逐个读入字符然后返回，并且不改变文件流的内容（猜测应该是作为一个非常基本的组件供语法分析调用）
int peek(FILE *in) {
	int c;

	c = getc(in);
	ungetc(c, in);//将从文件中读入的字符c再写回文件流in中
	return c;
}

//将从file中读入的流去除空格和注释然后返回到读入流中去：应该是作为文本预处理
void eat_whitespace(FILE *in) {
	int c;

	//没有换行符就一直按字符读入
	while ((c = getc(in)) != EOF) {
		//如果是空格字符继续读入，也就是忽略了空格字符
		if (isspace(c)) {
			continue;//结束本次循环，回到外部while再次循环
		}
		//如果是分号，表示后续内容是注释，也忽略
		else if (c == ';') { /* comments are whitespace also */
			while (((c = getc(in)) != EOF) && (c != '\n'));//除非遇到换行，不然继续循环读入
			continue;
		}
		//把一个字符退回到输入流中，这时候的文件流中就去除了空格字符和注释
		ungetc(c, in);
		break;
	}
}

//从输入文件流in中比对str指定的字符，如果没有就报错
void eat_expected_string(FILE *in, char *str) {
	int c;

	while (*str != '\0') {
		//一个字符一个字符的比较（有点像字符串匹配的BN方式：参见../PractiseMakePerfect工程）
		c = getc(in);
		if (c != *str) {
			fprintf(stderr, "unexpected character '%c'\n", c);
			exit(1);
		}
		str++;//指针后移一位
	}
}

//判断当前输入文件流中是否存在界定符外的符号，如果有的话报错（应该是已经去除了注释:eat_whitespace，然后再是这一步）
void peek_expected_delimiter(FILE *in) {
	if (!is_delimiter(peek(in))) {
		fprintf(stderr, "character not followed by delimiter\n");
		exit(1);
	}
}

//按照字符读入文件中的源代码并且判断
object *read_character(FILE *in) {
	int c;

	c = getc(in);
	//这儿使用这种方式来判断读入文件，是出于什么原因才这样写？？？？见object *read(FILE *in)函数，用于读入转义字符
	//首先判断当前从文件流中获取的字符串；
	//然后在不影响后续文件流的情况下判断是否有后续的字符p或者e，如果有就继续读入，最终判断是否为space或者newline字符
	//最后执行对应的内部object对象构造，否则执行一般的字符类型object对象构造
	switch (c) {
	case EOF:
		fprintf(stderr, "incomplete character literal\n");
		exit(1);
	case 's':
		if (peek(in) == 'p') {
			eat_expected_string(in, "pace");//判断如果输入了space，就构造一个空格字符
			peek_expected_delimiter(in);
			return make_character(' ');
		}
		break;
	case 'n':
		if (peek(in) == 'e') {
			eat_expected_string(in, "ewline");
			peek_expected_delimiter(in);
			return make_character('\n');
		}
		break;
	}
	peek_expected_delimiter(in);
	//将读入的字符转换为内部CHARACTER类型object对象
	return make_character(c);
}

//从文件中读入序对类型
object *read_pair(FILE *in) {
	int c;
	object *car_obj;
	object *cdr_obj;
	//去除注释等内容
	eat_whitespace(in);
	//读入字符
	c = getc(in);
	//如果读入字符为右括号就判断为空序对，这儿是不是应该判断括号的匹配问题？（参见../PractiseMakePerfect工程）
	if (c == ')') { /* read the empty list */
		return the_empty_list;
	}
	ungetc(c, in);

	car_obj = read(in);

	eat_whitespace(in);

	c = getc(in);
	if (c == '.') { /* read improper list */
		c = peek(in);
		if (!is_delimiter(c)) {
			fprintf(stderr, "dot not followed by delimiter\n");
			exit(1);
		}
		cdr_obj = read(in);
		eat_whitespace(in);
		c = getc(in);
		if (c != ')') {
			fprintf(stderr,
				"where was the trailing right paren?\n");
			exit(1);
		}
		return cons(car_obj, cdr_obj);
	}
	else { /* read list */
		ungetc(c, in);
		cdr_obj = read_pair(in);
		return cons(car_obj, cdr_obj);
	}
}

//从读入文件构造基本object：外部文件交互的核心函数
object *read(FILE *in) {
	int c;
	short sign = 1;
	int i;
	long num = 0;
#define BUFFER_MAX 1000
	char buffer[BUFFER_MAX];

	//去除注释等无用符号
	eat_whitespace(in);

	//从in中读入一个字符
	c = getc(in);

	//#开头的是布尔类型
	if (c == '#') { /* read a boolean or character */
		c = getc(in);
		switch (c) {
		case 't':
			return true;
		case 'f':
			return false;
		case '\\':
			return read_character(in);//读入#\\表示后续是个转义？需要进一步判断
		default:
			fprintf(stderr,
				"unknown boolean or character literal\n");
			exit(1);
		}
	}
	//判断是否是数字，包括负数
	else if (isdigit(c) || (c == '-' && (isdigit(peek(in))))) {
		/* read a fixnum */
		if (c == '-') {
			sign = -1;
		}
		else {
			ungetc(c, in);
		}
		while (isdigit(c = getc(in))) {
			num = (num * 10) + (c - '0');//因为通过getc读入，所以需要减掉字符'0'
		}
		num *= sign;//确定符号
		if (is_delimiter(c)) {
			ungetc(c, in);
			return make_fixnum(num);
		}
		else {
			fprintf(stderr, "number not followed by delimiter\n");
			exit(1);
		}
	}
	//判断是否是开始符
	else if (is_initial(c) || ( (c == '+' || c == '-') && is_delimiter(peek(in)) ) ) { /* read a symbol */
		i = 0;
		while (is_initial(c) || isdigit(c) ||
			c == '+' || c == '-') {
			/* subtract 1 to save space for '\0' terminator */
			if (i < BUFFER_MAX - 1) {
				buffer[i++] = c;
			}
			else {
				fprintf(stderr, "symbol too long. "
					"Maximum length is %d\n", BUFFER_MAX);
				exit(1);
			}
			c = getc(in);
		}
		if (is_delimiter(c)) {
			buffer[i] = '\0';
			ungetc(c, in);
			return make_symbol(buffer);
		}
		else {
			fprintf(stderr, "symbol not followed by delimiter. "
				"Found '%c'\n", c);
			exit(1);
		}
	}
	//判断是否是字符串开头"
	else if (c == '"') { /* read a string */
		i = 0;
		while ((c = getc(in)) != '"') {
			if (c == '\\') {
				c = getc(in);
				if (c == 'n') {
					c = '\n';
				}
			}
			if (c == EOF) {
				fprintf(stderr, "non-terminated string literal\n");
				exit(1);
			}
			/* subtract 1 to save space for '\0' terminator */
			if (i < BUFFER_MAX - 1) {
				buffer[i++] = c;
			}
			else {
				fprintf(stderr,
					"string too long. Maximum length is %d\n",
					BUFFER_MAX);
				exit(1);
			}
		}
		buffer[i] = '\0';
		return make_string(buffer);
	}
	//左括号开头的是序对
	else if (c == '(') { /* read the empty list or pair */
		return read_pair(in);
	}
	//斜杠和'开头的是引用
	else if (c == '\'') { /* read quoted expression */
		return cons(quote_symbol, cons(read(in), the_empty_list));
	}
	//空
	else if (c == EOF) {
		return NULL;
	}
	//其他情况说明文件读入错误，不予处理
	else {
		fprintf(stderr, "bad input. Unexpected '%c'\n", c);
		exit(1);
	}
	fprintf(stderr, "read illegal state\n");
	exit(1);
}

/*************************** EVALUATE ****************************/
//以下内容是表达式求值部分，在表达式的构造基础上做出正确处理

//自求值类型直接返回对应的值
char is_self_evaluating(object *exp) {
	return is_boolean(exp) ||
		is_fixnum(exp) ||
		is_character(exp) ||
		is_string(exp);
}

//变量就是符号
char is_variable(object *expression) {
	return is_symbol(expression);
}

//基本函数：检测输入的expression是否具和tag类型相同
//expression是一个序对，第一个元素就是对应tag的一个符号；
//tag符号在init函数中构造
char is_tagged_list(object *expression, object *tag) {
	object *the_car;

	if (is_pair(expression)) {
		the_car = car(expression);//操作的第一个元素就是操作本身的说明
		return is_symbol(the_car) && (the_car == tag);
	}
	return 0;
}

//判断是否为引用
char is_quoted(object *expression) {
	return is_tagged_list(expression, quote_symbol);
}

//获取引用的文字部分
object *text_of_quotation(object *exp) {
	return cadr(exp);
}

//判断是否是赋值类型
char is_assignment(object *exp) {
	return is_tagged_list(exp, set_symbol);
}

//获取赋值中的变量
object *assignment_variable(object *exp) {
	return car(cdr(exp));//形式为（set! a 1），也就是序对：(set! (a 1))，所以这样获取
}

//获取赋值中的值，object类型
object *assignment_value(object *exp) {
	return car(cdr(cdr(exp)));
}

//是否是定义：关键在于定义式的形式是什么？(define (a 1))，和赋值是相同的
char is_definition(object *exp) {
	return is_tagged_list(exp, define_symbol);
}

//获取定义中的变量
object *definition_variable(object *exp) {
	if (is_symbol(cadr(exp))) {//这个样子是怎么获取的？？？
		return cadr(exp);
	}
	else {
		return caadr(exp);
	}
}

//构造lambda函数
object *make_lambda(object *parameters, object *body);

//获取定义中的值：如果定义中不是直接值，就需要构造lambda函数来返回整个object类型的函数体
object *definition_value(object *exp) {
	if (is_symbol(cadr(exp))) {
		return caddr(exp);
	}
	else {
		//这儿返回的也是object类型的函数体
		return make_lambda(cdadr(exp), cddr(exp));
	}
}

//通过cons构造序对嵌套的if结构：
//predicate条件为真执行consequent，否则执行alternative
object *make_if(object *predicate, object *consequent, object *alternative) {
	return cons(if_symbol,
		cons(predicate,
		cons(consequent,
		cons(alternative, the_empty_list))));//这个构造说明了if的内部结构
}

//判断是否是if结构对象
char is_if(object *expression) {
	return is_tagged_list(expression, if_symbol);
}

//获取if结构的条件判断object对象
object *if_predicate(object *exp) {
	return cadr(exp);
}

//获取if结构的正确执行分支
object *if_consequent(object *exp) {
	return caddr(exp);
}

//获取if结构的错误执行分支
object *if_alternative(object *exp) {
	if (is_the_empty_list(cdddr(exp))) {
		return false;
	}
	else {
		return cadddr(exp);
	}
}

//通过序对方式构造lambda函数
object *make_lambda(object *parameters, object *body) {
	return cons(lambda_symbol, cons(parameters, body));
}

//判断是否是lambda函数
char is_lambda(object *exp) {
	return is_tagged_list(exp, lambda_symbol);
}

//获取lambda的参数列表
object *lambda_parameters(object *exp) {
	return cadr(exp);
}

//获取lambda的函数体
object *lambda_body(object *exp) {
	return cddr(exp);
}

//构造begin结构
object *make_begin(object *seq) {
	return cons(begin_symbol, seq);
}

char is_begin(object *exp) {
	return is_tagged_list(exp, begin_symbol);
}

object *begin_actions(object *exp) {
	return cdr(exp);
}

//通过判断seq的后继是否为空，如果为空就表明当前的seq是表达式的最后一个
char is_last_exp(object *seq) {
	return is_the_empty_list(cdr(seq));
}

object *first_exp(object *seq) {
	return car(seq);
}

object *rest_exps(object *seq) {
	return cdr(seq);
}

char is_cond(object *exp) {
	return is_tagged_list(exp, cond_symbol);
}

//获取cond的从句（clauses）
object *cond_clauses(object *exp) {
	return cdr(exp);
}

object *cond_predicate(object *clause) {
	return car(clause);
}

//
object *cond_actions(object *clause) {
	return cdr(clause);
}

//判断是否是cond的else从句
char is_cond_else_clause(object *clause) {
	return cond_predicate(clause) == else_symbol;
}

//用输入序列构造表达式
object *sequence_to_exp(object *seq) {
	if (is_the_empty_list(seq)) {
		return seq;//直接返回空的对象
	}
	else if (is_last_exp(seq)) {
		return first_exp(seq);//如果seq后继为空，就传回第一个
	}
	else {
		return make_begin(seq);//构造begin表达式返回
	}
}

//扩展从句
object *expand_clauses(object *clauses) {
	object *first;
	object *rest;

	if (is_the_empty_list(clauses)) {
		return false;
	}
	else {
		first = car(clauses);
		rest = cdr(clauses);
		//如果第一个元素（first）是else从句
		if (is_cond_else_clause(first)) {
			//如果rest元素为空，就表明可以在这儿后继
			if (is_the_empty_list(rest)) {
				return sequence_to_exp(cond_actions(first));
			}
			//否则表明cond未完结，不能在这儿加入条件扩招
			else {
				fprintf(stderr, "else clause isn't last cond->if");
				exit(1);
			}
		}
		//如果第一个元素（first）不是else从句，调用make_if构造
		else {
			return make_if(cond_predicate(first),
				sequence_to_exp(cond_actions(first)),
				expand_clauses(rest));
		}
	}
}

//cond转换为if
object *cond_to_if(object *exp) {
	return expand_clauses(cond_clauses(exp));
}

//构造应用：还是一个序对，第一个对象是操作符，第二个对象时操作数
object *make_application(object *operator, object *operands) {
	return cons(operator, operands);
}

//应用本身就是一个序对，也是这样判断的
char is_application(object *exp) {
	return is_pair(exp);
}

//获取application的操作符
object *operator(object *exp) {
	return car(exp);
}

//获取application的操作数
object *operands(object *exp) {
	return cdr(exp);
}

//operands是否为空判断
char is_no_operands(object *ops) {
	return is_the_empty_list(ops);
}

object *first_operand(object *ops) {
	return car(ops);
}

object *rest_operands(object *ops) {
	return cdr(ops);
}

//let对象判断
char is_let(object *exp) {
	return is_tagged_list(exp, let_symbol);
}

object *let_bindings(object *exp) {
	return cadr(exp);
}

object *let_body(object *exp) {
	return cddr(exp);
}

//获取被绑定的参数列表：alpha变换相关
object *binding_parameter(object *binding) {
	return car(binding);
}

//获取被绑定的参数值
object *binding_argument(object *binding) {
	return cadr(binding);
}

object *bindings_parameters(object *bindings) {
	return is_the_empty_list(bindings) ?
	the_empty_list :
				   cons(binding_parameter(car(bindings)),
				   bindings_parameters(cdr(bindings)));
}

object *bindings_arguments(object *bindings) {
	return is_the_empty_list(bindings) ?
	the_empty_list :
				   cons(binding_argument(car(bindings)),
				   bindings_arguments(cdr(bindings)));
}

//获取let的参数列表
object *let_parameters(object *exp) {
	return bindings_parameters(let_bindings(exp));
}

//获取let的参数值列表
object *let_arguments(object *exp) {
	return bindings_arguments(let_bindings(exp));
}

//将let表达式通过调用lambda函数来构造application
object *let_to_application(object *exp) {
	return make_application(
		make_lambda(let_parameters(exp), let_body(exp)),//let的参数列表和函数体构造lambda函数
		let_arguments(exp));//然后将let的参数值构造lambda的调用
}

char is_and(object *exp) {
	return is_tagged_list(exp, and_symbol);
}

//测试？
object *and_tests(object *exp) {
	return cdr(exp);
}

char is_or(object *exp) {
	return is_tagged_list(exp, or_symbol);
}

object *or_tests(object *exp) {
	return cdr(exp);
}

//获取apply的运算符
object *apply_operator(object *arguments) {
	return car(arguments);
}

//递归完成apply的运算符构建
object *prepare_apply_operands(object *arguments) {
	if (is_the_empty_list(cdr(arguments))) {
		return car(arguments);//递归的终止条件
	}
	else {
		return cons(car(arguments),
			prepare_apply_operands(cdr(arguments)));//递归调用
	}
}

//获取apply的运算符列表
object *apply_operands(object *arguments) {
	return prepare_apply_operands(cdr(arguments));
}

//获取表达式？作用不清楚
object *eval_expression(object *arguments) {
	return car(arguments);
}

object *eval_environment(object *arguments) {
	return cadr(arguments);
}

//递归：先对第一个操作符在环境env下求值，然后将后续操作符构建递归求值操作
object *list_of_values(object *exps, object *env) {
	if (is_no_operands(exps)) {
		return the_empty_list;//终止条件
	}
	else {
		return cons(eval(first_operand(exps), env),
			list_of_values(rest_operands(exps), env));//构造递归
	}
}

//对赋值求值：将变量的值和变量符号绑定在环境env中（可以看见一个变量的赋值过程是如何完成的，很不错）
object *eval_assignment(object *exp, object *env) {
	set_variable_value(assignment_variable(exp),
		eval(assignment_value(exp), env),
		env);
	return ok_symbol;
}

//在环境env中对定义表达式exp求值，就是完成定义的关联
object *eval_definition(object *exp, object *env) {
	define_variable(definition_variable(exp),
		eval(definition_value(exp), env),
		env);
	return ok_symbol;
}

//核心函数：在环境env中对表达式exp求值
//lisp的根本就是一个循环的求值函数，scheme中的严格尾递归就是带参数传递的goto
object *eval(object *exp, object *env) {
	object *procedure;//过程体
	object *arguments;//参数列表
	object *result;//结果集

//尾递归标志，因为表达式可能有多个部分，需要循环求值
tailcall:
	//如果是自求值类型就返回本身
	if (is_self_evaluating(exp)) {
		return exp;
	}
	//如果是变量就在环境中查找对应的值
	else if (is_variable(exp)) {
		return lookup_variable_value(exp, env);
	}
	//如果是引用就返回除引用符号外的文字部分
	else if (is_quoted(exp)) {
		return text_of_quotation(exp);
	}
	//如果是赋值就在环境中将变量与值关联，返回成功
	else if (is_assignment(exp)) {
		return eval_assignment(exp, env);
	}
	//如果是定义就在环境中将变量与值关联，返回成功，和上面类型，但是有区分
	else if (is_definition(exp)) {
		return eval_definition(exp, env);
	}
	//如果是if表达式，判断条件是否为真，然后计算结果返回为一个表达式，进入下一次尾递归处理
	else if (is_if(exp)) {
		exp = is_true(eval(if_predicate(exp), env)) ?
			if_consequent(exp) :
			if_alternative(exp);
		goto tailcall;
	}
	//如果是lambda表达式，则返回由lambda参数构建的复合过程
	else if (is_lambda(exp)) {
		return make_compound_proc(lambda_parameters(exp),
			lambda_body(exp),
			env);
	}
	//如果是begin表达式，就从begin的条件开始判断，然后将后续选项在环境中逐一求值
	else if (is_begin(exp)) {
		exp = begin_actions(exp);
		while (!is_last_exp(exp)) {
			eval(first_exp(exp), env);
			exp = rest_exps(exp);
		}
		exp = first_exp(exp);
		goto tailcall;
	}
	//如果是cond表达式，转换为if表达式，然后进入尾递归循环
	else if (is_cond(exp)) {
		exp = cond_to_if(exp);
		goto tailcall;
	}
	//如果是let表达式，构造为应用返回，然后进入尾递归
	else if (is_let(exp)) {
		exp = let_to_application(exp);
		goto tailcall;
	}
	//如果是and表达式
	else if (is_and(exp)) {
		//先获取and测试表达式部分
		exp = and_tests(exp);
		//如果当前表达式为空就直接返回真
		if (is_the_empty_list(exp)) {
			return true;
		}
		//如果是最后的表达式元素，就从第一个元素开始求值
		while (!is_last_exp(exp)) {
			result = eval(first_exp(exp), env);
			if (is_false(result)) {
				return result;
			}
			exp = rest_exps(exp);
		}
		exp = first_exp(exp);
		goto tailcall;
	}
	//如果是or表达式，就先获取测试部分表达式，和and处理相同
	else if (is_or(exp)) {
		exp = or_tests(exp);
		if (is_the_empty_list(exp)) {
			return false;
		}
		while (!is_last_exp(exp)) {
			result = eval(first_exp(exp), env);
			if (is_true(result)) {
				return result;
			}
			exp = rest_exps(exp);
		}
		exp = first_exp(exp);
		goto tailcall;
	}
	//如果是application表达式
	else if (is_application(exp)) {
		procedure = eval(operator(exp), env);//获取application的操作符并且在环境env中求值，作为过程体
		arguments = list_of_values(operands(exp), env);//获取application的操作值并且在环境env中求值，作为参数列表

		/* handle eval specially for tail call requirement */
		//如果是基本过程并且过程体执行不能被求值（通过判断指针是否相同达到）
		if (is_primitive_proc(procedure) &&
			procedure->data.primitive_proc.fn == eval_proc) {
			//重新设置表达式和环境，然后进入尾递归循环
			exp = eval_expression(arguments);
			env = eval_environment(arguments);
			goto tailcall;
		}

		/* handle apply specially for tail call requirement */
		//如果是基本过程并且过程体执行不能被执行（通过判断指针是否相同达到）
		if (is_primitive_proc(procedure) &&
			procedure->data.primitive_proc.fn == apply_proc) {
			procedure = apply_operator(arguments);
			arguments = apply_operands(arguments);
		}

		//如果是基本过程，就调用函数指针传入参数完成调用
		if (is_primitive_proc(procedure)) {
			return (procedure->data.primitive_proc.fn)(arguments);
		}
		//如果是复合过程，就获取当前的环境和表达式，然后进入尾递归循环继续求值
		else if (is_compound_proc(procedure)) {
			env = extend_environment(
				procedure->data.compound_proc.parameters,
				arguments,
				procedure->data.compound_proc.env);
			exp = make_begin(procedure->data.compound_proc.body);
			goto tailcall;
		}
		//其他类型的过程不被识别，直接报错
		else {
			fprintf(stderr, "unknown procedure type\n");
			exit(1);
		}
	}
	//其他类型输入不被识别，无法求值
	else {
		fprintf(stderr, "cannot eval unknown expression type\n");
		exit(1);
	}
	//如果求值不能正确解析，报错
	fprintf(stderr, "eval illegal state\n");
	exit(1);
}

/**************************** PRINT ******************************/
//以下内容是打印部分，用于和用户交互显示和文件操作

//将序对pair写入到out指定的文件中
//(a(b c)) = > a(b c) = > a b.c
void write_pair(FILE *out, object *pair) {
	object *car_obj;
	object *cdr_obj;

	car_obj = car(pair);
	cdr_obj = cdr(pair);
	//先将序对的第一个元素写入文件
	write(out, car_obj);
	//然后判断序对的后继元素，如果后继又是一个序对，用空格分割然后递归调用。
	if (cdr_obj->type == PAIR) {
		fprintf(out, " ");
		write_pair(out, cdr_obj);
	}
	//如果后继为空，直接返回
	else if (cdr_obj->type == THE_EMPTY_LIST) {
		return;
	}
	//如果后继就是一个单元，用.分割后写入文件
	else {
		fprintf(out, " . ");
		write(out, cdr_obj);
	}
}

//将object对象写入out指定的文件中
void write(FILE *out, object *obj) {
	char c;
	char *str;

	switch (obj->type) {
	//空列表表示为()
	case THE_EMPTY_LIST:
		fprintf(out, "()");
		break;
	//布尔类型表示为t或者f
	case BOOLEAN:
		fprintf(out, "#%c", is_false(obj) ? 'f' : 't');
		break;
	//符号类型按照对应的值直接输出
	case SYMBOL:
		fprintf(out, "%s", obj->data.symbol.value);
		break;
	//数值类型按照long int输出
	case FIXNUM:
		fprintf(out, "%ld", obj->data.fixnum.value);
		break;
	//字符类型按照#\\开头然后输出，所以可以表达转义字符（这里就和读入文件的格式相同）
	case CHARACTER:
		c = obj->data.character.value;
		fprintf(out, "#\\");
		switch (c) {
		case '\n':
			fprintf(out, "newline");
			break;
		case ' ':
			fprintf(out, "space");
			break;
		default:
			putc(c, out);
		}
		break;
	//字符串类型需要用"包围，然后转义将值输出为格式文件
	case STRING:
		str = obj->data.string.value;
		putchar('"');
		while (*str != '\0') {
			switch (*str) {
			case '\n':
				fprintf(out, "\\n");
				break;
			case '\\':
				fprintf(out, "\\\\");
				break;
			case '"':
				fprintf(out, "\\\"");
				break;
			default:
				putc(*str, out);
			}
			str++;
		}
		putchar('"');
		break;
	//序对用左右括号包围输出
	case PAIR:
		fprintf(out, "(");
		write_pair(out, obj);
		fprintf(out, ")");
		break;
	//基本过程用#<primitive-procedure>标记开头
	case PRIMITIVE_PROC:
		fprintf(out, "#<primitive-procedure>");
		break;
	//复合过程用#<compound-procedure>标记开头
	case COMPOUND_PROC:
		fprintf(out, "#<compound-procedure>");
		break;
	//输入类型用#<input-port>标记开头
	case INPUT_PORT:
		fprintf(out, "#<input-port>");
		break;
	//输出类型用#<output-port>标记开头
	case OUTPUT_PORT:
		fprintf(out, "#<output-port>");
		break;
	//结尾符号用#<eof>标记开头
	case EOF_OBJECT:
		fprintf(out, "#<eof>");
		break;
	//其他的不被识别
	default:
		fprintf(stderr, "cannot write unknown type\n");
		exit(1);
	}
}

/***************************** REPL ******************************/

int main(void) {
	object *exp;

	printf("Welcome to Bootstrap Scheme. "
		"Use ctrl-c to exit.\n");

	init();

	while (1) {
		printf("> ");
		exp = read(stdin);
		if (exp == NULL) {
			break;
		}
		write(stdout, eval(exp, the_global_environment));
		printf("\n");
	}

	printf("Goodbye\n");

	return 0;
}

/**************************** MUSIC *******************************

Slipknot, Neil Young, Pearl Jam, The Dead Weather,
Dave Matthews Band, Alice in Chains, White Zombie, Blind Melon,
Priestess, Puscifer, Bob Dylan, Them Crooked Vultures,
Black Sabbath, Pantera, Tool, ZZ Top, Queens of the Stone Age,
Raised Fist, Rage Against the Machine, Primus, Black Label Society,
The Offspring, Nickelback, Metallica, Jeff Beck, M.I.R.V.,
The Tragically Hip, Willie Nelson, Highwaymen

*/