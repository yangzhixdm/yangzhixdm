---
title:  JavaScript 实现继承.
date: 2015-08-18 11:45:24
tags:
description: javascript基础，关于继承。
---
### 原型继承
对于javascript中的继承，由于js中没有后端语言中的类式继承，所以js中的继承，一般都是原型继承(prototype)。
``` javascript
function P (name){
	this.name = name;
	this.say = function(){
		console.log('p');
	}
}

function S (name,id){
	this.id = id;

	this.eat = function(){
		console.log('s');
	}
}

S.prototype = P.prototype;

var s = new S();
```

以上代代码存在一个重要问题：
``` javascript
S.prototype = P.prototype;
```
该句代码会导致，在修改子类的prototype的时候，同样会影响父类的prototype元素。

解决方式一般采用new 父级对象的方式：
``` javascript
S.prototype = new P();
```
这里涉及到new 的工作原理，new P(), 会根据P的prototype对象建立一个实例，然后将构造方法中的成员变量和成员方法设置进去。

此种方式， 修改子类的prototype则不会影响父类的prototpye(由于是新建了一个对象，不再是指向同一个prototype).
设置prototype之后，还需要重置子类S的contrustor,让其重指向 S，而不是new P() 所对应的construstor. 如下：
``` javascript
S.prototype.constructor = S;
```
很多时候这里不重置，不会出现问题，但是如果后面需要用到constructor 来创建实例，或者判定实例类型，则会出现错误。
所以这里最好是重置。

上面所述的原型继承，虽然可以通过JavaScript原型继承，但是同样存在不足。
此种继承方式，不能够继承在构造方法设置的成员变量和成员方法，只能继承在原型中设置的方法，或者属性。
如：
``` javascript
var s = new S('yang','01');
```
其中的第一个参数，并不能传递给到name属性。
于是引出第二种继承方式： 对象冒充。
### 对象冒充继承

``` javascript
function P (name){
	this.name = name;
	this.say = function(){
		console.log('p');
	}
}

function S (name,id){
	P.call(this,name);
	this.id = id;
	this.eat = function(){
		console.log('s');
	}
}

var s = new S('yanghi','test');
```

在子类构造方法中，借用父类的构造方法，使子类具有父类的属性和方法。如下：
``` javascript
P.call(this,name);
```
该句代码与如下效果一样：
``` javascript
this.name = name;
this.say = function(){}
```
从而将父类的属性拷贝到子类中，从而实现对象冒充。

那么该此种对象冒充方式，同样存在问题:
  1 无法继承原型prototype中的属性和主法。
  2 构造方法的成员方法在父类和子类中都会有一份拷贝，造成内存的增加。

所以最好的实现方式是：
1 对原型对象，采用原型继承

2 构造方法中的属性和方法，采用对象冒充。

这也是当前绝大对数的js继承库所采用的实现方式。如下：

``` javascript
function P (name){
	this.name = name;
}

P.prototype.say = function(){
	console.log('say');
}

function S (name,id){
	P.call(this,name);
	this.id = id;
}

function Bare(){}
Bare.prototype = P.prototype;
S.prototype = new Bare();
S.prototype.constructor = S;

S.prototype.eat = function(){
	console.log('eat');
}

var s = new S('yanghi','test');
```
这里的成员属性，采用对象冒充，成员方法采用原型继承。

注意一点，这里实现原型继承需要采用一个中间变量，如下：
``` javascript
S.prototype = new Bare();
```
如果不采用中间变量，直接new P() 的话，会出现问题。
由于new 会按照P的prototype对象模版，创建一个对象，这一步没有问题。
但是接下来，它会将P构造方法的成员属性也设置到这个对象中，就会导致对这个对象污染。
这里我们只需要它的prototype就可以了，其它的成员变量采用对象冒充的方式就可以了。

第三种实现继承方式，采用ES5 Object.create实现。

### Object.create

``` javascript
//Shape - superclass
function Shape() {
  this.x = 0;
  this.y = 0;
}

Shape.prototype.move = function(x, y) {
    this.x += x;
    this.y += y;
    console.info("Shape moved.");
};

// Rectangle - subclass
function Rectangle() {
  Shape.call(this); //call super constructor.
}

Rectangle.prototype = Object.create(Shape.prototype);

var rect = new Rectangle();

rect instanceof Rectangle //true.
rect instanceof Shape //true.

rect.move(1, 1); //Outputs, "Shape moved."
```

不过此方式最大的问题还是兼容性，需要 IE 9 + , safari 5 +,opera 11.6 + 

以上就是对象继承的一些问题，以记录之，谨防忘记。