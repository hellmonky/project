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
���ڴ�unixϵͳ��ֲ����������C��������ʾ����ȫ��VS2013�������C4996�������ʽΪ��
��Ŀ->����->��������->C/C++ -> Ԥ������ -> Ԥ���������壬����_CRT_SECURE_NO_DEPRECATE
��дע��һ��Ҫע�⣺�ȶ�����������Ȼ���ٱ���������������ã����������
*/

/*
��һ�������ϡ��ڿ��Զ�����ֲ�ͬ���������ͣ� һ����˵��Ϊ�á����ϡ����͵ı����У�����װ��á����ϡ���������κ�һ�����ݣ���Щ���ݹ���ͬһ���ڴ棬�Ѵﵽ��ʡ�ռ��Ŀ�ģ�����һ����ʡ�ռ�����ͣ�λ�򣩡�
����һ���ǳ�����ĵط���Ҳ�����ϵ����������⣬ͬstructһ��������Ĭ�Ϸ���Ȩ��Ҳ�ǹ��еģ����ң�Ҳ���г�Ա������
���ǽṹ������ϵ��ڴ�ֲ��ǲ�ͬ�ģ�
�ڽṹ�и���Ա�и��Ե��ڴ�ռ䣬 һ���ṹ�������ܳ����Ǹ���Ա����֮�ͣ��սṹ���⣬ͬʱ�����Ǳ߽��������
���ڡ����ϡ��У�����Ա����һ���ڴ�ռ䣬 һ�����ϱ����ĳ��ȵ��ڸ���Ա����ĳ��ȡ�
Ӧ��˵�����ǣ� ������ν�Ĺ�����ָ�Ѷ����Աͬʱװ��һ�����ϱ����ڣ� ����ָ�����ϱ����ɱ�������һ��Աֵ����ÿ��ֻ�ܸ�һ��ֵ�� ������ֵ���ȥ��ֵ��
*/

/*
make_symbol��define������š�
�������������ַ���һ����Ҳ�����������͵�һ�֡������Լ��������ʾ��ʽ��literal representation�����ܹ������ڱ����У�Ҳ�ܵ����������ݡ�һ����������Ӧ���еĹ��ܣ������С�
���ţ�symbol�����͡�����ʵ������һ��ָ�룬ָ�򴢴��ڹ�ϣ���е��ַ��������ԣ��Ƚ����������Ƿ���ȣ�ֻҪ�����ǵ�ָ���Ƿ�һ�������ˣ���������ַ��رȽϡ�
�ܶ���֮�����еĶ�������һ�����������scheme��lisp��������ϵͳ��
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/**************************** MODEL ******************************/
//���������ǻ���ģ�ͽ�������

//�������Ļ������ͣ�ʹ��ö�����ͣ���Ϊ������ȡֵ���޶���һ�����޵ķ�Χ�ڡ�Ĭ�ϴ�0��ʼ
typedef enum {
	THE_EMPTY_LIST, BOOLEAN, SYMBOL, FIXNUM,
	CHARACTER, STRING, PAIR, PRIMITIVE_PROC,
	COMPOUND_PROC, INPUT_PORT, OUTPUT_PORT,
	EOF_OBJECT
} object_type;

//����ṹ�壺�������ݽṹ�����̲�����Ψһ���ͣ�ϵͳ���õĺ��û��Զ����û������
//����ͱ�����ʽ���ͣ���Ϊû�б��������ͣ�ֻ�ж�������ͣ�Ҳ����˵���ͺͶ��������
//�����������͵ı����ĸ��һ�ж��ǻ��������ʵ����
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
		} pair;//��ԣ�ʹ��˫ָ�룬����������lisp�������Ϊ�˴�������
		struct {
			struct object *(*fn)(struct object *arguments);
		} primitive_proc;//�������̣�����Ϊ�������͵ĺ���ָ��
		struct {
			struct object *parameters;
			struct object *body;
			struct object *env;
		} compound_proc;//���Ϲ��̣�ʹ�ò����б�������ָ�루����ָ�룩�ͻ���ָ��
		struct {
			FILE *stream;
		} input_port;//�����ļ����ļ���ָ��
		struct {
			FILE *stream;
		} output_port;//����ļ����ļ���ָ��
	} data;
} object;//�ⲿʹ�ýṹ�����øö�������ͺ�ֵ���ڲ�ʹ�������������ɲ�ͬ��ֵ����ʵ��

//û�м���GC���ƣ�������Ҫһֱ�����ڴ�����֤���С�GC���Ƶļ���Ƚϸ��ӣ������и�����������ۡ�
/* no GC so truely "unlimited extent" */
object *alloc_object(void) {
	object *obj;

	obj = malloc(sizeof(object));//ÿ�ζ�����һ�������С���ڴ棬û��ʹ���κ��ڴ������ļ���
	if (obj == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}
	return obj;
}

//������������
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
//�����������ù���
object *cons(object *car, object *cdr);
object *car(object *pair);
object *cdr(object *pair);

//�ù�ϵ�������==��������Ƿ���ͬ������char���ͽ������ΪC89û���ڽ���bool���ͣ�
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

//���Ź��캯�������������ַ���ָ���ķ������ͣ�ͨ�����symbol_table�з��ж�Ӧ�������������������͵Ķ��󡣣�Ȼ�󷵻�������͡�
//963�����ڹ����������
object *make_symbol(char *value) {
	object *obj;
	object *element;

	//���Ȼ�ȡ��ǰ�����Ͷ����symbol_table��һ��ȫ��ά����������˵����ǰϵͳ�д��ڵ�����
	element = symbol_table;

	/* search for they symbol in the symbol table */
	//�����ǰ�Ķ������ͱ�symbol_table��Ϊ�գ�Ҳ����˵��ǰ�Ѿ��������Ͷ��壬��ô�Ͳ���������Ͷ��������е������Ƿ��valueָ��������ͬ��
	while (!is_the_empty_list(element)) {
		//�����ͬ�ͷ�����������
		if (strcmp(car(element)->data.symbol.value, value) == 0) {
			return car(element);//����ƥ������ͣ�Ҳ��һ������
		}
		//�����ͬ��ʹ��cdr��ȡsymbol_table���к��������ͣ��������
		element = cdr(element);
	};

	/* create the symbol and add it to the symbol table */
	//�����ǰ�Ķ������ͱ�symbol_tableΪ�գ���ʾ��ǰû�����Ͷ��壬��ô�ͽ�������ͼ��뵽symbol_table���С�
	obj = alloc_object();
	obj->type = SYMBOL;
	obj->data.symbol.value = malloc(strlen(value) + 1);//Ϊ��������������ռ�
	if (obj->data.symbol.value == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}
	strcpy(obj->data.symbol.value, value);//�����������͵�������͵�ר������obj�У���Ϊһ�ж���object�����ַ��������һ��object��
	symbol_table = cons(obj, symbol_table);//Ȼ��������������ӵ����е�symbol_table��
	return obj;//���ص�ǰ���Ͷ���
}

//��⵱ǰ�����Ƿ�Ϊһ��symbol
char is_symbol(object *obj) {
	return obj->type == SYMBOL;//����������Ƿ���SYMBOL��ö��ֻ����һ����ǰֵ
}

//����fixnum����ϵͳ�ṩ�Ļ������ͣ����Բ���Ҫ��鵱ǰsymbol_table���Ƿ��ж�Ӧ����
object *make_fixnum(long value) {
	object *obj;

	obj = alloc_object();
	obj->type = FIXNUM;//ֱ��ָ������
	obj->data.fixnum.value = value;
	return obj;//��󷵻صĻ���һ��object�������û��Զ������ͺ�ϵͳ�ṩ������û������
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
	obj->data.string.value = malloc(strlen(value) + 1);//��Ҫ����ռ������ã�������һ����Ϊ�˴�Ž�����
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

//�������̣�������ԣ�ͨ��ָ�����ϳ�һ���ṹ
object *cons(object *car, object *cdr) {
	object *obj;

	obj = alloc_object();
	obj->type = PAIR;//Ĭ�Ϲ�����һ�����
	obj->data.pair.car = car;
	obj->data.pair.cdr = cdr;
	return obj;
}

char is_pair(object *obj) {
	return obj->type == PAIR;
}

//��ȡ��Եĵ�һ��Ԫ�أ���Ĭ�ϴ��������Խṹ����ȡdata�ṹ�еĵ�һ��ָ��
object *car(object *pair) {
	return pair->data.pair.car;
}

void set_car(object *obj, object* value) {
	obj->data.pair.car = value;
}

//��ȡ��Ե����һ��Ԫ�أ���Ĭ�ϴ��������Խṹ����ȡdata�ṹ�е�ĩβָ��
object *cdr(object *pair) {
	return pair->data.pair.cdr;
}

void set_cdr(object *obj, object* value) {
	obj->data.pair.cdr = value;
}

//����������ַ���ƥ�����Զ����죬����һ������Ҳ����ֱ��ʹ��Ƕ�׽�������car��cdr
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

//����������̣�ͨ������һ��ָ����̵�ָ�빹��������̣����ش��й������͵�object����
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
ʹ��car��cdr����ȡ�����жϣ�Ҳ����˵���ｫproc����һ����ԣ�
���������object���ɣ���һ���ǹ����壬�ڶ����ǹ��̵Ĳ�����
*/

//ͨ���������object�ĵ�һ��Ԫ���Ƿ�Ϊ����ȷ���Ƿ�Ϊ�չ��̣�����ж���Щ���⣩
object *is_null_proc(object *arguments) {
	return is_the_empty_list(car(arguments)) ? true : false;
}

//ͨ���������object�ĵ�һ��Ԫ�ص��������жϹ����Ƿ�����������
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

//ͨ�����ͱ�ǩ����Ƿ�Ϊ���Ϲ���
char is_compound_proc(object *obj);

//�������object�Ƿ���һ�����̣�������������primitive_proc�͸��Ϲ���compound_proc
object *is_procedure_proc(object *arguments) {
	object *obj;

	obj = car(arguments);//Ĭ������ԣ���ȡobject�ĵ�һ������
	return (is_primitive_proc(obj) ||
		is_compound_proc(obj)) ?
		true :
		false;
}

//����ת����������ȡ����ǰobject��ֵ��Ȼ����ڶ�Ӧ���͵�ֵ���У�����ָ��������൱��ǿ������ת��
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
һ�¹����漰����list�ı�����Ӧ�ó���Ϊmap���̣�����������Ҫ�����Ĺ��̺�Ҫִ�еĲ���
*/

//�������̵Ķ��壺��һ��list��ÿ����Ա��ֵ�ۼӲ��ҷ�����������object
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

//������е�һ��object�������������������̵�Ŀ����ʲô
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

//�жϵ�ǰobject������Ԫ��ֵ�Ƿ���ͬ��Ҳ��������map����д����������
object *is_number_equal_proc(object *arguments) {
	long value;

	value = (car(arguments))->data.fixnum.value;//��ȡ��ǰ����object��ֵ
	while (!is_the_empty_list(arguments = cdr(arguments))) {//���Ƚ���ǰobject�ĺ�̸�ֵ��object�������Ҽ�����Ƿ�Ϊ�գ������Ϊ�ս���ѭ��
		if (value != ((car(arguments))->data.fixnum.value)) {//��ȡ��ǰ��objectֵ���һ�δ����objectֵ���бȽϣ������ͬ�ͷ��أ���������Ƚ�
			return false;
		}
	}
	return true;
	/*����Լ�д��
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

//�ж�һ��list�ǲ����ɴ�С����˳��ֵ����
object *is_less_than_proc(object *arguments) {
	long previous;
	long next;

	previous = (car(arguments))->data.fixnum.value;//��ȡ��ǰobject��ֵ
	while (!is_the_empty_list(arguments = cdr(arguments))) {
		next = (car(arguments))->data.fixnum.value;//��ȡʣ��list�е�һ��object��ֵ
		if (previous < next) {//���ǰ��С�ں�̣������ֵ��ֵ��ǰ����Ȼ������жϺ���
			previous = next;
		}
		else {//���ǰ�����ں�̾ͷ��ش���
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

//��һ�����̹���Ϊ��ԣ�������̵�Ŀ����ʲô��
object *cons_proc(object *arguments) {
	return cons(car(arguments), cadr(arguments));
}

//��ȡ���̵ĵ�һ��object����
object *car_proc(object *arguments) {
	return caar(arguments);
}

//��ȡ���̵ĳ���һ������object����
object *cdr_proc(object *arguments) {
	return cdar(arguments);
}

//������object�ĺ���object������Ϊֵ��ֵ����һ��object��������pair�ĵ�һ����������Ҫ���̣����Կ������̲�������
object *set_car_proc(object *arguments) {
	set_car(car(arguments), cadr(arguments));
	return ok_symbol;
}

//������object�ĺ���object������Ϊֵ��ֵ����һ��object��������pair�ĵڶ���������
object *set_cdr_proc(object *arguments) {
	set_cdr(car(arguments), cadr(arguments));
	return ok_symbol;
}

object *list_proc(object *arguments) {
	return arguments;
}

//Ĭ��proc����ԣ��Ƚ��������Ԫ�������Ƿ���ͬ
object *is_eq_proc(object *arguments) {
	object *obj1;
	object *obj2;

	obj1 = car(arguments);
	obj2 = cadr(arguments);

	//�Ƚ���Ե�һ���ͺ���Ԫ�ص������Ƿ���ͬ
	if (obj1->type != obj2->type) {
		return false;
	}
	//����ֵ�����ͣ���ֵ����������жϣ�Ȼ�󷵻��Ƿ���ȣ�����������Ϊʹ�������ϣ�����Ҫ��ÿһ���������������
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

//�������̲������С���Ϊ�������̵ļ����̱����ã����õķ�ʽ�ǶԱ���������ָ���Ƿ���ͬ��Ӧ�û���һ����ֵ������жϺ͸�ֵ
object *apply_proc(object *arguments) {
	fprintf(stderr, "illegal state: The body of the apply "
		"primitive procedure should not execute.\n");
	exit(1);
}

//���ص�ǰ����������������scheme�зǳ���Ҫ�ĸ���
object *interaction_environment_proc(object *arguments) {
	return the_global_environment;//���ص�ǰȫ�ֻ���
}

//��ʼ��һ���ջ���������ȫ�ֽ����������һ����������������ĵ��á����о�д�ıȼ۲ִ٣��������̻��ǿ��Ը��Ƶģ�
object *setup_environment(void);

//��ȷָ����ʼ��һ���ջ���
object *null_environment_proc(object *arguments) {
	return setup_environment();
}

//����һ�������������ӹ���evalʱ�Ļ�������
object *make_environment(void);

//ͨ������������컷�������ǹ�����һ���µĿջ�������������˼·�й��ˣ�
object *environment_proc(object *arguments) {
	return make_environment();
}

//�������̲�����ֵ����Ϊ�������̵ļ����̱����ã����õķ�ʽ�ǶԱ���������ָ���Ƿ���ͬ��Ӧ�û���һ����ֵ������жϺ͸�ֵ
object *eval_proc(object *arguments) {
	fprintf(stderr, "illegal state: The body of the eval "
		"primitive procedure should not execute.\n");
	exit(1);
}

//�ļ��������������ļ����룬����ɹ���ת��Ϊobject����
object *read(FILE *in);
//��ֵ���̣��ñ��ʽ�ͻ���������ֵ�����Ĺ���
object *eval(object *exp, object *env);

//������̣��ļ�������ϸ�����̣����ڲ����ļ��ж���Ĺ���
object *load_proc(object *arguments) {
	char *filename;
	FILE *in;
	object *exp;
	object *result = NULL;

	filename = car(arguments)->data.string.value;//����object�е�ֵ��ΪҪ������ļ�������
	in = fopen(filename, "r");//���˳�����ļ��ͷ���ָ��������ļ�ָ��
	if (in == NULL) {
		fprintf(stderr, "could not load file \"%s\"", filename);
		exit(1);
	}
	while ((exp = read(in)) != NULL) {//���ļ�ָ������ļ����������Ϊ�գ�û�н��������Ͷ�һ�䣬ִ��һ��
		result = eval(exp, the_global_environment);//ʹ���ⲿ��������
	}
	fclose(in);//�ر��ļ�
	return result;
}

//ͨ���ļ����������ļ����͵�object��ϵͳ֧�ֵĻ������ͣ�
object *make_input_port(FILE *in);

//��������ļ�����object
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

//�ر������ļ����͵�object
object *close_input_port_proc(object *arguments) {
	int result;

	result = fclose(car(arguments)->data.input_port.stream);
	if (result == EOF) {
		fprintf(stderr, "could not close input port\n");
		exit(1);
	}
	return ok_symbol;
}

//�ж�object�Ƿ�Ϊ�����ļ�����
char is_input_port(object *obj);
object *is_input_port_proc(object *arguments) {
	return is_input_port(car(arguments)) ? true : false;
}

//������object�������
object *read_proc(object *arguments) {
	FILE *in;
	object *result;

	//�ж�����object�Ƿ�Ϊ�գ����Ϊ�վʹ�stdin���루��׼C���뺯�������ڴӼ��̻�ȡ��������������ʹ��ļ�����
	in = is_the_empty_list(arguments) ?
	stdin :
		  car(arguments)->data.input_port.stream;//����object��Ϊ��ʱ����object��һ������ָ�����ļ���ֵ��in
	result = read(in);
	return (result == NULL) ? eof_object : result;
}

//������object������̣�Ȼ��ת��Ϊcharacter����object���
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
//��ȡ��ǰ����objectָ���ļ����ߵ�ǰ�������������Ȼ��ת��Ϊcharacter����object���
object *peek_char_proc(object *arguments) {
	FILE *in;
	int result;

	in = is_the_empty_list(arguments) ?
	stdin :
		  car(arguments)->data.input_port.stream;
	result = peek(in);
	return (result == EOF) ? eof_object : make_character(result);//��Ҫ�ǿռ��������
}

//�������object�Ƿ�����������жϵ�ǰ�Ӽ��̻����ļ�����Ĺ����Ƿ����
char is_eof_object(object *obj);

object *is_eof_object_proc(object *arguments) {
	return is_eof_object(car(arguments)) ? true : false;
}

//ͨ�������ļ�������д����object
object *make_output_port(FILE *in);

object *open_output_port_proc(object *arguments) {
	char *filename;
	FILE *out;

	filename = car(arguments)->data.string.value;
	out = fopen(filename, "w");//дģʽ
	if (out == NULL) {
		fprintf(stderr, "could not open file \"%s\"\n", filename);
		exit(1);
	}
	return make_output_port(out);
}

object *close_output_port_proc(object *arguments) {
	int result;

	result = fclose(car(arguments)->data.output_port.stream);//�ر�object��ָ�����ļ�
	if (result == EOF) {
		fprintf(stderr, "could not close output port\n");
		exit(1);
	}
	return ok_symbol;
}

//���object�Ƿ�Ϊ�����ļ�����
char is_output_port(object *obj);

object *is_output_port_proc(object *arguments) {
	return is_output_port(car(arguments)) ? true : false;
}

//
object *write_char_proc(object *arguments) {
	object *character;
	FILE *out;

	character = car(arguments);//����һ��������ֵ��character
	arguments = cdr(arguments);//��̲������Ǳ���
	out = is_the_empty_list(arguments) ?//���û�к�̲������ʹӼ��̻�ȡ���룬����Ӻ�̲�����ȡ����ļ�
	stdout :
		   car(arguments)->data.output_port.stream;
	putc(character->data.character.value, out);//����һ������д�뵽����ļ���
	fflush(out);//�Ǳ�׼���������������д����������Ҫ��������������������ݽ�������д��ʱʹ��
	return ok_symbol;
}

//��object�е�����ת�壨ת��Ϊ�˿���ʶ��ķ��ţ�д�뵽�ļ���ȥ��Ҳ���ǽ���������objectд���ļ��У�Ҳ���ڽ����е���ʾ
void write(FILE *out, object *obj);

//�������object����Ȼ��д�뵽�������ڻ����ļ��У�stdout��
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

//����������������object�������մ��������������ڲ������������ⲿ��ʾ�Ľ������̣����ڽ��ڲ��жϴ���Ĺ�����ʾ���û�
object *error_proc(object *arguments) {
	while (!is_the_empty_list(arguments)) {
		write(stderr, car(arguments));
		fprintf(stderr, " ");
		arguments = cdr(arguments);
	};
	printf("\nexiting\n");
	exit(1);
}

//���츴�Ϲ��̣�parameters�����б�body������ָ�룬env����ָ��
object *make_compound_proc(object *parameters, object *body, object* env) {
	object *obj;

	obj = alloc_object();//����һ���յ�object��Ȼ��������
	obj->type = COMPOUND_PROC;
	obj->data.compound_proc.parameters = parameters;
	obj->data.compound_proc.body = body;
	obj->data.compound_proc.env = env;
	return obj;//����Ҳ�Ƕ��󣬿��Ա����أ��������ݼ����룬���뼴����
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

//���ػ����ĺ�̣������ڱհ�������
object *enclosing_environment(object *env) {
	return cdr(env);
}

/*
һ���������ǿ�ܵ�һ�����У�ÿ������ǰ�����һЩԼ����һ����񣨿���Ϊ�գ���
��ЩԼ����һЩ���������ֹ�������Ӧ��ֵ�ϣ�һ������У��κα������ֻ����һ��Լ����Ҳ����˵һ������б�����ֵ��Ψһ�ģ�
ÿ����ܻ�������һ��ָ����������Χ������ָ�롣

�ۺ�������
��1������ֵ�Ļ���ģ���£��������ǿ�ܵļ��ϣ�
��2�������һ��key-value������˵����������ֵ�Ķ�Ӧ��ϵ��
��3�����֮����ڲ�ι�ϵ������ͨ��ָ����������ֲ�ι�ϵ˵���˱�����������
��3���ڻ����в��ұ�����ֵ��Ҫ����ι�ϵ�ӿ���в��ң���Ϊ��Լ����Ч�ʺܵͣ����﷨�������ǰ��
*/

//��ȡ�����ĵ�һ��������Ҳ�������frame
object *first_frame(object *env) {
	return car(env);
}

//һ��frame����һ����ԣ���ʾΪkey-value��ϵ��������map�����ڱ�����ֵ�Ķ�Ӧ��ϵ
object *make_frame(object *variables, object *values) {
	return cons(variables, values);
}

//��ȡframe��key
object *frame_variables(object *frame) {
	return car(frame);
}

//��ȡframe��value
object *frame_values(object *frame) {
	return cdr(frame);
}

//ͨ��������Եķ�ʽ��key-value�󶨵�frame�ϣ�Ҳ�������һ�������Ķ��嵽frame��
void add_binding_to_frame(object *var, object *val, object *frame) {
	set_car(frame, cons(var, car(frame)));
	set_cdr(frame, cons(val, cdr(frame)));
}

//��չ���������һ���µ�frame��ԭ�е�env��ȥ���ؼ������µ�frame��env�Ķ���
object *extend_environment(object *vars, object *vals, object *base_env) {
	return cons(make_frame(vars, vals), base_env);
}

//��ȱ������ұ�����ֵ����ֵ�Ļ���ģ�ͣ���ѯ����ʱ��Ҫ����frame����𲽲�ѯ
object *lookup_variable_value(object *var, object *env) {
	object *frame;
	object *vars;
	object *vals;
	while (!is_the_empty_list(env)) {
		frame = first_frame(env);
		vars = frame_variables(frame);//��ȡ��ǰ����frame�����б����б�
		vals = frame_values(frame);//��ȡ��ǰ����frame�����б�����Ӧֵ���б�
		while (!is_the_empty_list(vars)) {//��ǰframe��Ϊ��ʱ��ʼ����
			if (var == car(vars)) {//�ӵ�һ��Ԫ�ؿ�ʼ����
				return car(vals);
			}
			vars = cdr(vars);//���û�У����δӺ���Ԫ�ز���
			vals = cdr(vals);
		}
		env = enclosing_environment(env);//��ǰframeû�У�����һ��frame����
	}
	fprintf(stderr, "unbound variable, %s\n", var->data.symbol.value);//��ָ���������޷��ҵ���������
	exit(1);
}

//��ָ�����������ñ�����ֵ������ȱ��������ҵ��ñ����ĵ�һ��frame������ֵ
//ע�⣺����Ӧ���������ĵģ�������Щ�ֲڣ��������һ��frame����ͬ�������������ȡ�᣿
//����Ĵ���ʽ���ڶ������ʱ����ָ���Ļ�����
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

//�����Ķ��壺ʹ��defineʱӦ�õ��õĹ��̣�ָ����������ֵ�������Ļ���
void define_variable(object *var, object *val, object *env) {
	object *frame;
	object *vars;
	object *vals;

	//�ڻ����Ķ���frame��ӱ���
	frame = first_frame(env);
	vars = frame_variables(frame);
	vals = frame_values(frame);

	//����ڵ�ǰframe��������������������¸�ֵ
	while (!is_the_empty_list(vars)) {
		if (var == car(vars)) {
			set_car(vals, val);
			return;
		}
		vars = cdr(vars);
		vals = cdr(vals);
	}
	//���������µı�����ֵ��
	add_binding_to_frame(var, val, frame);
}

//���û�������ʼ��һ���յĻ���
object *setup_environment(void) {
	object *initial_env;

	initial_env = extend_environment(
		the_empty_list,
		the_empty_list,
		the_empty_environment);
	return initial_env;
}

//��ָ��������ע��������̣���Ϊlisp�����ж���������define������һ��object�����Ի���������Ҫ�ڳ�ʼ�����н���ע�ᣬȻ��ʹ��
void populate_environment(object *env) {

//ע�������scheme���̺Ͷ�Ӧ�������Ĺ�����scheme_name���û����õ�scheme���̣�c_name������ļ���ʵ�ֵĹ���
//ע�᷽ʽ������define_variable���̣���scheme_nameע��Ϊ���ţ�Ҳ����string������c_nameͨ��make_primitive_proc������������̣����ҹ���ȫ��env
#define add_procedure(scheme_name, c_name)              \
    define_variable(make_symbol(scheme_name),           \
                    make_primitive_proc(c_name),        \
                    env);

	//��һЩ�������̼��뵽�����У�Ȼ��ʹ��symbol_table������
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

//��������������һ���յĳ�ʼ�������ͻ������̣������Ϳ��������������ʹ���ṩ�Ĺ���
object *make_environment(void) {
	object *env;

	env = setup_environment();
	populate_environment(env);
	return env;
}

//��ʼ�����̺���������˶Ի�������ͻ������̵Ĺ���
void init(void) {
	//���ȹ����������
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

	//Ȼ����ȫ�ֻ���
	the_empty_environment = the_empty_list;

	//��󹹽�һ�������л������̵�ȫ�ֻ���
	the_global_environment = make_environment();
}

/***************************** READ ******************************/
//�����������ı�����������֣������ڱ���ԭ���еĴʷ����������μ�CompilersImplementation\UCC_WIN��Ŀ��

//�綨���жϣ�lisp�еĽ綨���������ո��ַ����������������ţ������ţ����ţ��ֺš�������Щ�綨�����������Ž�����ʶ��
//��Ҫע�������������ƥ������
char is_delimiter(int c) {
	return isspace(c) || c == EOF ||
		c == '(' || c == ')' ||
		c == '"' || c == ';';
}

//�ж��Ƿ��ǿ�ʼ���ţ�lispҲ������乹�ɵģ���ô����Ҫ�ж϶�����ַ��Ƿ������Ŀ�ʼ
char is_initial(int c) {
	//isalpha�����ж��ַ�c�Ƿ�ΪӢ����ĸa-z��A-Z���Ǿͷ��ط���ֵ(��һ����1)�����򷵻���
	return isalpha(c) || c == '*' || c == '/' || c == '>' ||
		c == '<' || c == '=' || c == '?' || c == '!';
}

/*
ungetc�ǽ������������ٴηŻص�������ȥ����һ�ζ�����ʱ�����ٴζ������ġ�
ungetc()���������ã���������ж����ַ�����ȴ��������Ҫ������ʱ��Ϊ�˲����������ݣ�Ҳ������������ã�����Ҫ��д������
д�غ��ٶ����Կ�������������
*/
//��ȡ�ӵ�ǰ�ļ�����������ַ�Ȼ�󷵻أ����Ҳ��ı��ļ��������ݣ��²�Ӧ������Ϊһ���ǳ�������������﷨�������ã�
int peek(FILE *in) {
	int c;

	c = getc(in);
	ungetc(c, in);//�����ļ��ж�����ַ�c��д���ļ���in��
	return c;
}

//����file�ж������ȥ���ո��ע��Ȼ�󷵻ص���������ȥ��Ӧ������Ϊ�ı�Ԥ����
void eat_whitespace(FILE *in) {
	int c;

	//û�л��з���һֱ���ַ�����
	while ((c = getc(in)) != EOF) {
		//����ǿո��ַ��������룬Ҳ���Ǻ����˿ո��ַ�
		if (isspace(c)) {
			continue;//��������ѭ�����ص��ⲿwhile�ٴ�ѭ��
		}
		//����Ƿֺţ���ʾ����������ע�ͣ�Ҳ����
		else if (c == ';') { /* comments are whitespace also */
			while (((c = getc(in)) != EOF) && (c != '\n'));//�����������У���Ȼ����ѭ������
			continue;
		}
		//��һ���ַ��˻ص��������У���ʱ����ļ����о�ȥ���˿ո��ַ���ע��
		ungetc(c, in);
		break;
	}
}

//�������ļ���in�бȶ�strָ�����ַ������û�оͱ���
void eat_expected_string(FILE *in, char *str) {
	int c;

	while (*str != '\0') {
		//һ���ַ�һ���ַ��ıȽϣ��е����ַ���ƥ���BN��ʽ���μ�../PractiseMakePerfect���̣�
		c = getc(in);
		if (c != *str) {
			fprintf(stderr, "unexpected character '%c'\n", c);
			exit(1);
		}
		str++;//ָ�����һλ
	}
}

//�жϵ�ǰ�����ļ������Ƿ���ڽ綨����ķ��ţ�����еĻ�����Ӧ�����Ѿ�ȥ����ע��:eat_whitespace��Ȼ��������һ����
void peek_expected_delimiter(FILE *in) {
	if (!is_delimiter(peek(in))) {
		fprintf(stderr, "character not followed by delimiter\n");
		exit(1);
	}
}

//�����ַ������ļ��е�Դ���벢���ж�
object *read_character(FILE *in) {
	int c;

	c = getc(in);
	//���ʹ�����ַ�ʽ���ж϶����ļ����ǳ���ʲôԭ�������д����������object *read(FILE *in)���������ڶ���ת���ַ�
	//�����жϵ�ǰ���ļ����л�ȡ���ַ�����
	//Ȼ���ڲ�Ӱ������ļ�����������ж��Ƿ��к������ַ�p����e������оͼ������룬�����ж��Ƿ�Ϊspace����newline�ַ�
	//���ִ�ж�Ӧ���ڲ�object�����죬����ִ��һ����ַ�����object������
	switch (c) {
	case EOF:
		fprintf(stderr, "incomplete character literal\n");
		exit(1);
	case 's':
		if (peek(in) == 'p') {
			eat_expected_string(in, "pace");//�ж����������space���͹���һ���ո��ַ�
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
	//��������ַ�ת��Ϊ�ڲ�CHARACTER����object����
	return make_character(c);
}

//���ļ��ж����������
object *read_pair(FILE *in) {
	int c;
	object *car_obj;
	object *cdr_obj;
	//ȥ��ע�͵�����
	eat_whitespace(in);
	//�����ַ�
	c = getc(in);
	//��������ַ�Ϊ�����ž��ж�Ϊ����ԣ�����ǲ���Ӧ���ж����ŵ�ƥ�����⣿���μ�../PractiseMakePerfect���̣�
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

//�Ӷ����ļ��������object���ⲿ�ļ������ĺ��ĺ���
object *read(FILE *in) {
	int c;
	short sign = 1;
	int i;
	long num = 0;
#define BUFFER_MAX 1000
	char buffer[BUFFER_MAX];

	//ȥ��ע�͵����÷���
	eat_whitespace(in);

	//��in�ж���һ���ַ�
	c = getc(in);

	//#��ͷ���ǲ�������
	if (c == '#') { /* read a boolean or character */
		c = getc(in);
		switch (c) {
		case 't':
			return true;
		case 'f':
			return false;
		case '\\':
			return read_character(in);//����#\\��ʾ�����Ǹ�ת�壿��Ҫ��һ���ж�
		default:
			fprintf(stderr,
				"unknown boolean or character literal\n");
			exit(1);
		}
	}
	//�ж��Ƿ������֣���������
	else if (isdigit(c) || (c == '-' && (isdigit(peek(in))))) {
		/* read a fixnum */
		if (c == '-') {
			sign = -1;
		}
		else {
			ungetc(c, in);
		}
		while (isdigit(c = getc(in))) {
			num = (num * 10) + (c - '0');//��Ϊͨ��getc���룬������Ҫ�����ַ�'0'
		}
		num *= sign;//ȷ������
		if (is_delimiter(c)) {
			ungetc(c, in);
			return make_fixnum(num);
		}
		else {
			fprintf(stderr, "number not followed by delimiter\n");
			exit(1);
		}
	}
	//�ж��Ƿ��ǿ�ʼ��
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
	//�ж��Ƿ����ַ�����ͷ"
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
	//�����ſ�ͷ�������
	else if (c == '(') { /* read the empty list or pair */
		return read_pair(in);
	}
	//б�ܺ�'��ͷ��������
	else if (c == '\'') { /* read quoted expression */
		return cons(quote_symbol, cons(read(in), the_empty_list));
	}
	//��
	else if (c == EOF) {
		return NULL;
	}
	//�������˵���ļ�������󣬲��账��
	else {
		fprintf(stderr, "bad input. Unexpected '%c'\n", c);
		exit(1);
	}
	fprintf(stderr, "read illegal state\n");
	exit(1);
}

/*************************** EVALUATE ****************************/
//���������Ǳ��ʽ��ֵ���֣��ڱ��ʽ�Ĺ��������������ȷ����

//����ֵ����ֱ�ӷ��ض�Ӧ��ֵ
char is_self_evaluating(object *exp) {
	return is_boolean(exp) ||
		is_fixnum(exp) ||
		is_character(exp) ||
		is_string(exp);
}

//�������Ƿ���
char is_variable(object *expression) {
	return is_symbol(expression);
}

//������������������expression�Ƿ�ߺ�tag������ͬ
//expression��һ����ԣ���һ��Ԫ�ؾ��Ƕ�Ӧtag��һ�����ţ�
//tag������init�����й���
char is_tagged_list(object *expression, object *tag) {
	object *the_car;

	if (is_pair(expression)) {
		the_car = car(expression);//�����ĵ�һ��Ԫ�ؾ��ǲ��������˵��
		return is_symbol(the_car) && (the_car == tag);
	}
	return 0;
}

//�ж��Ƿ�Ϊ����
char is_quoted(object *expression) {
	return is_tagged_list(expression, quote_symbol);
}

//��ȡ���õ����ֲ���
object *text_of_quotation(object *exp) {
	return cadr(exp);
}

//�ж��Ƿ��Ǹ�ֵ����
char is_assignment(object *exp) {
	return is_tagged_list(exp, set_symbol);
}

//��ȡ��ֵ�еı���
object *assignment_variable(object *exp) {
	return car(cdr(exp));//��ʽΪ��set! a 1����Ҳ������ԣ�(set! (a 1))������������ȡ
}

//��ȡ��ֵ�е�ֵ��object����
object *assignment_value(object *exp) {
	return car(cdr(cdr(exp)));
}

//�Ƿ��Ƕ��壺�ؼ����ڶ���ʽ����ʽ��ʲô��(define (a 1))���͸�ֵ����ͬ��
char is_definition(object *exp) {
	return is_tagged_list(exp, define_symbol);
}

//��ȡ�����еı���
object *definition_variable(object *exp) {
	if (is_symbol(cadr(exp))) {//�����������ô��ȡ�ģ�����
		return cadr(exp);
	}
	else {
		return caadr(exp);
	}
}

//����lambda����
object *make_lambda(object *parameters, object *body);

//��ȡ�����е�ֵ����������в���ֱ��ֵ������Ҫ����lambda��������������object���͵ĺ�����
object *definition_value(object *exp) {
	if (is_symbol(cadr(exp))) {
		return caddr(exp);
	}
	else {
		//������ص�Ҳ��object���͵ĺ�����
		return make_lambda(cdadr(exp), cddr(exp));
	}
}

//ͨ��cons�������Ƕ�׵�if�ṹ��
//predicate����Ϊ��ִ��consequent������ִ��alternative
object *make_if(object *predicate, object *consequent, object *alternative) {
	return cons(if_symbol,
		cons(predicate,
		cons(consequent,
		cons(alternative, the_empty_list))));//�������˵����if���ڲ��ṹ
}

//�ж��Ƿ���if�ṹ����
char is_if(object *expression) {
	return is_tagged_list(expression, if_symbol);
}

//��ȡif�ṹ�������ж�object����
object *if_predicate(object *exp) {
	return cadr(exp);
}

//��ȡif�ṹ����ȷִ�з�֧
object *if_consequent(object *exp) {
	return caddr(exp);
}

//��ȡif�ṹ�Ĵ���ִ�з�֧
object *if_alternative(object *exp) {
	if (is_the_empty_list(cdddr(exp))) {
		return false;
	}
	else {
		return cadddr(exp);
	}
}

//ͨ����Է�ʽ����lambda����
object *make_lambda(object *parameters, object *body) {
	return cons(lambda_symbol, cons(parameters, body));
}

//�ж��Ƿ���lambda����
char is_lambda(object *exp) {
	return is_tagged_list(exp, lambda_symbol);
}

//��ȡlambda�Ĳ����б�
object *lambda_parameters(object *exp) {
	return cadr(exp);
}

//��ȡlambda�ĺ�����
object *lambda_body(object *exp) {
	return cddr(exp);
}

//����begin�ṹ
object *make_begin(object *seq) {
	return cons(begin_symbol, seq);
}

char is_begin(object *exp) {
	return is_tagged_list(exp, begin_symbol);
}

object *begin_actions(object *exp) {
	return cdr(exp);
}

//ͨ���ж�seq�ĺ���Ƿ�Ϊ�գ����Ϊ�վͱ�����ǰ��seq�Ǳ��ʽ�����һ��
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

//��ȡcond�ĴӾ䣨clauses��
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

//�ж��Ƿ���cond��else�Ӿ�
char is_cond_else_clause(object *clause) {
	return cond_predicate(clause) == else_symbol;
}

//���������й�����ʽ
object *sequence_to_exp(object *seq) {
	if (is_the_empty_list(seq)) {
		return seq;//ֱ�ӷ��ؿյĶ���
	}
	else if (is_last_exp(seq)) {
		return first_exp(seq);//���seq���Ϊ�գ��ʹ��ص�һ��
	}
	else {
		return make_begin(seq);//����begin���ʽ����
	}
}

//��չ�Ӿ�
object *expand_clauses(object *clauses) {
	object *first;
	object *rest;

	if (is_the_empty_list(clauses)) {
		return false;
	}
	else {
		first = car(clauses);
		rest = cdr(clauses);
		//�����һ��Ԫ�أ�first����else�Ӿ�
		if (is_cond_else_clause(first)) {
			//���restԪ��Ϊ�գ��ͱ���������������
			if (is_the_empty_list(rest)) {
				return sequence_to_exp(cond_actions(first));
			}
			//�������condδ��ᣬ���������������������
			else {
				fprintf(stderr, "else clause isn't last cond->if");
				exit(1);
			}
		}
		//�����һ��Ԫ�أ�first������else�Ӿ䣬����make_if����
		else {
			return make_if(cond_predicate(first),
				sequence_to_exp(cond_actions(first)),
				expand_clauses(rest));
		}
	}
}

//condת��Ϊif
object *cond_to_if(object *exp) {
	return expand_clauses(cond_clauses(exp));
}

//����Ӧ�ã�����һ����ԣ���һ�������ǲ��������ڶ�������ʱ������
object *make_application(object *operator, object *operands) {
	return cons(operator, operands);
}

//Ӧ�ñ������һ����ԣ�Ҳ�������жϵ�
char is_application(object *exp) {
	return is_pair(exp);
}

//��ȡapplication�Ĳ�����
object *operator(object *exp) {
	return car(exp);
}

//��ȡapplication�Ĳ�����
object *operands(object *exp) {
	return cdr(exp);
}

//operands�Ƿ�Ϊ���ж�
char is_no_operands(object *ops) {
	return is_the_empty_list(ops);
}

object *first_operand(object *ops) {
	return car(ops);
}

object *rest_operands(object *ops) {
	return cdr(ops);
}

//let�����ж�
char is_let(object *exp) {
	return is_tagged_list(exp, let_symbol);
}

object *let_bindings(object *exp) {
	return cadr(exp);
}

object *let_body(object *exp) {
	return cddr(exp);
}

//��ȡ���󶨵Ĳ����б�alpha�任���
object *binding_parameter(object *binding) {
	return car(binding);
}

//��ȡ���󶨵Ĳ���ֵ
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

//��ȡlet�Ĳ����б�
object *let_parameters(object *exp) {
	return bindings_parameters(let_bindings(exp));
}

//��ȡlet�Ĳ���ֵ�б�
object *let_arguments(object *exp) {
	return bindings_arguments(let_bindings(exp));
}

//��let���ʽͨ������lambda����������application
object *let_to_application(object *exp) {
	return make_application(
		make_lambda(let_parameters(exp), let_body(exp)),//let�Ĳ����б�ͺ����幹��lambda����
		let_arguments(exp));//Ȼ��let�Ĳ���ֵ����lambda�ĵ���
}

char is_and(object *exp) {
	return is_tagged_list(exp, and_symbol);
}

//���ԣ�
object *and_tests(object *exp) {
	return cdr(exp);
}

char is_or(object *exp) {
	return is_tagged_list(exp, or_symbol);
}

object *or_tests(object *exp) {
	return cdr(exp);
}

//��ȡapply�������
object *apply_operator(object *arguments) {
	return car(arguments);
}

//�ݹ����apply�����������
object *prepare_apply_operands(object *arguments) {
	if (is_the_empty_list(cdr(arguments))) {
		return car(arguments);//�ݹ����ֹ����
	}
	else {
		return cons(car(arguments),
			prepare_apply_operands(cdr(arguments)));//�ݹ����
	}
}

//��ȡapply��������б�
object *apply_operands(object *arguments) {
	return prepare_apply_operands(cdr(arguments));
}

//��ȡ���ʽ�����ò����
object *eval_expression(object *arguments) {
	return car(arguments);
}

object *eval_environment(object *arguments) {
	return cadr(arguments);
}

//�ݹ飺�ȶԵ�һ���������ڻ���env����ֵ��Ȼ�󽫺��������������ݹ���ֵ����
object *list_of_values(object *exps, object *env) {
	if (is_no_operands(exps)) {
		return the_empty_list;//��ֹ����
	}
	else {
		return cons(eval(first_operand(exps), env),
			list_of_values(rest_operands(exps), env));//����ݹ�
	}
}

//�Ը�ֵ��ֵ����������ֵ�ͱ������Ű��ڻ���env�У����Կ���һ�������ĸ�ֵ�����������ɵģ��ܲ���
object *eval_assignment(object *exp, object *env) {
	set_variable_value(assignment_variable(exp),
		eval(assignment_value(exp), env),
		env);
	return ok_symbol;
}

//�ڻ���env�жԶ�����ʽexp��ֵ��������ɶ���Ĺ���
object *eval_definition(object *exp, object *env) {
	define_variable(definition_variable(exp),
		eval(definition_value(exp), env),
		env);
	return ok_symbol;
}

//���ĺ������ڻ���env�жԱ��ʽexp��ֵ
//lisp�ĸ�������һ��ѭ������ֵ������scheme�е��ϸ�β�ݹ���Ǵ��������ݵ�goto
object *eval(object *exp, object *env) {
	object *procedure;//������
	object *arguments;//�����б�
	object *result;//�����

//β�ݹ��־����Ϊ���ʽ�����ж�����֣���Ҫѭ����ֵ
tailcall:
	//���������ֵ���;ͷ��ر���
	if (is_self_evaluating(exp)) {
		return exp;
	}
	//����Ǳ������ڻ����в��Ҷ�Ӧ��ֵ
	else if (is_variable(exp)) {
		return lookup_variable_value(exp, env);
	}
	//��������þͷ��س����÷���������ֲ���
	else if (is_quoted(exp)) {
		return text_of_quotation(exp);
	}
	//����Ǹ�ֵ���ڻ����н�������ֵ���������سɹ�
	else if (is_assignment(exp)) {
		return eval_assignment(exp, env);
	}
	//����Ƕ�����ڻ����н�������ֵ���������سɹ������������ͣ�����������
	else if (is_definition(exp)) {
		return eval_definition(exp, env);
	}
	//�����if���ʽ���ж������Ƿ�Ϊ�棬Ȼ�����������Ϊһ�����ʽ��������һ��β�ݹ鴦��
	else if (is_if(exp)) {
		exp = is_true(eval(if_predicate(exp), env)) ?
			if_consequent(exp) :
			if_alternative(exp);
		goto tailcall;
	}
	//�����lambda���ʽ���򷵻���lambda���������ĸ��Ϲ���
	else if (is_lambda(exp)) {
		return make_compound_proc(lambda_parameters(exp),
			lambda_body(exp),
			env);
	}
	//�����begin���ʽ���ʹ�begin��������ʼ�жϣ�Ȼ�󽫺���ѡ���ڻ�������һ��ֵ
	else if (is_begin(exp)) {
		exp = begin_actions(exp);
		while (!is_last_exp(exp)) {
			eval(first_exp(exp), env);
			exp = rest_exps(exp);
		}
		exp = first_exp(exp);
		goto tailcall;
	}
	//�����cond���ʽ��ת��Ϊif���ʽ��Ȼ�����β�ݹ�ѭ��
	else if (is_cond(exp)) {
		exp = cond_to_if(exp);
		goto tailcall;
	}
	//�����let���ʽ������ΪӦ�÷��أ�Ȼ�����β�ݹ�
	else if (is_let(exp)) {
		exp = let_to_application(exp);
		goto tailcall;
	}
	//�����and���ʽ
	else if (is_and(exp)) {
		//�Ȼ�ȡand���Ա��ʽ����
		exp = and_tests(exp);
		//�����ǰ���ʽΪ�վ�ֱ�ӷ�����
		if (is_the_empty_list(exp)) {
			return true;
		}
		//��������ı��ʽԪ�أ��ʹӵ�һ��Ԫ�ؿ�ʼ��ֵ
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
	//�����or���ʽ�����Ȼ�ȡ���Բ��ֱ��ʽ����and������ͬ
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
	//�����application���ʽ
	else if (is_application(exp)) {
		procedure = eval(operator(exp), env);//��ȡapplication�Ĳ����������ڻ���env����ֵ����Ϊ������
		arguments = list_of_values(operands(exp), env);//��ȡapplication�Ĳ���ֵ�����ڻ���env����ֵ����Ϊ�����б�

		/* handle eval specially for tail call requirement */
		//����ǻ������̲��ҹ�����ִ�в��ܱ���ֵ��ͨ���ж�ָ���Ƿ���ͬ�ﵽ��
		if (is_primitive_proc(procedure) &&
			procedure->data.primitive_proc.fn == eval_proc) {
			//�������ñ��ʽ�ͻ�����Ȼ�����β�ݹ�ѭ��
			exp = eval_expression(arguments);
			env = eval_environment(arguments);
			goto tailcall;
		}

		/* handle apply specially for tail call requirement */
		//����ǻ������̲��ҹ�����ִ�в��ܱ�ִ�У�ͨ���ж�ָ���Ƿ���ͬ�ﵽ��
		if (is_primitive_proc(procedure) &&
			procedure->data.primitive_proc.fn == apply_proc) {
			procedure = apply_operator(arguments);
			arguments = apply_operands(arguments);
		}

		//����ǻ������̣��͵��ú���ָ�봫�������ɵ���
		if (is_primitive_proc(procedure)) {
			return (procedure->data.primitive_proc.fn)(arguments);
		}
		//����Ǹ��Ϲ��̣��ͻ�ȡ��ǰ�Ļ����ͱ��ʽ��Ȼ�����β�ݹ�ѭ��������ֵ
		else if (is_compound_proc(procedure)) {
			env = extend_environment(
				procedure->data.compound_proc.parameters,
				arguments,
				procedure->data.compound_proc.env);
			exp = make_begin(procedure->data.compound_proc.body);
			goto tailcall;
		}
		//�������͵Ĺ��̲���ʶ��ֱ�ӱ���
		else {
			fprintf(stderr, "unknown procedure type\n");
			exit(1);
		}
	}
	//�����������벻��ʶ���޷���ֵ
	else {
		fprintf(stderr, "cannot eval unknown expression type\n");
		exit(1);
	}
	//�����ֵ������ȷ����������
	fprintf(stderr, "eval illegal state\n");
	exit(1);
}

/**************************** PRINT ******************************/
//���������Ǵ�ӡ���֣����ں��û�������ʾ���ļ�����

//�����pairд�뵽outָ�����ļ���
//(a(b c)) = > a(b c) = > a b.c
void write_pair(FILE *out, object *pair) {
	object *car_obj;
	object *cdr_obj;

	car_obj = car(pair);
	cdr_obj = cdr(pair);
	//�Ƚ���Եĵ�һ��Ԫ��д���ļ�
	write(out, car_obj);
	//Ȼ���ж���Եĺ��Ԫ�أ�����������һ����ԣ��ÿո�ָ�Ȼ��ݹ���á�
	if (cdr_obj->type == PAIR) {
		fprintf(out, " ");
		write_pair(out, cdr_obj);
	}
	//������Ϊ�գ�ֱ�ӷ���
	else if (cdr_obj->type == THE_EMPTY_LIST) {
		return;
	}
	//�����̾���һ����Ԫ����.�ָ��д���ļ�
	else {
		fprintf(out, " . ");
		write(out, cdr_obj);
	}
}

//��object����д��outָ�����ļ���
void write(FILE *out, object *obj) {
	char c;
	char *str;

	switch (obj->type) {
	//���б��ʾΪ()
	case THE_EMPTY_LIST:
		fprintf(out, "()");
		break;
	//�������ͱ�ʾΪt����f
	case BOOLEAN:
		fprintf(out, "#%c", is_false(obj) ? 'f' : 't');
		break;
	//�������Ͱ��ն�Ӧ��ֱֵ�����
	case SYMBOL:
		fprintf(out, "%s", obj->data.symbol.value);
		break;
	//��ֵ���Ͱ���long int���
	case FIXNUM:
		fprintf(out, "%ld", obj->data.fixnum.value);
		break;
	//�ַ����Ͱ���#\\��ͷȻ����������Կ��Ա��ת���ַ�������ͺͶ����ļ��ĸ�ʽ��ͬ��
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
	//�ַ���������Ҫ��"��Χ��Ȼ��ת�彫ֵ���Ϊ��ʽ�ļ�
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
	//������������Ű�Χ���
	case PAIR:
		fprintf(out, "(");
		write_pair(out, obj);
		fprintf(out, ")");
		break;
	//����������#<primitive-procedure>��ǿ�ͷ
	case PRIMITIVE_PROC:
		fprintf(out, "#<primitive-procedure>");
		break;
	//���Ϲ�����#<compound-procedure>��ǿ�ͷ
	case COMPOUND_PROC:
		fprintf(out, "#<compound-procedure>");
		break;
	//����������#<input-port>��ǿ�ͷ
	case INPUT_PORT:
		fprintf(out, "#<input-port>");
		break;
	//���������#<output-port>��ǿ�ͷ
	case OUTPUT_PORT:
		fprintf(out, "#<output-port>");
		break;
	//��β������#<eof>��ǿ�ͷ
	case EOF_OBJECT:
		fprintf(out, "#<eof>");
		break;
	//�����Ĳ���ʶ��
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