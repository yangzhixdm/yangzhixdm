---
title: Flexbox布局总结
date: 2019-03-03
tags:
description: Flexbox布局总结-CSS权威指南第四版
---

以下内容来自CSS权威指南第四版本。

### 1 基本定义
> Flexbox works off of a parent and child relationship. Flexbox layout is activated by declaring  display: flex or  display: inline-flex on an element. This element becomes a flex container, arranging its children within the space provided and controlling their layout. The children of this flex container become flex items.

> Flexbox仅仅影响一对父子元素。可以通过对一个元素使用display:flex或者display: inlne-flex。那么这个元素就会变成一个flex容器，排列它的子元素根据被提供的间隔然后控制它们的布局。flex容器的字元素被称为flex item.
``` css

div#one {display: flex;}
div#two {display: inline-flex;}
div {border: 1px dashed; background: silver;}
div > * {border: 1px solid; background: #AAA;}
div p {margin: 0;}

<div id="one">
    <p>flex item with<br>two longer lines</p>
    <span>flex item</span>
    <p>flex item</p>
</div>
<div id="two">
    <span>flex item with<br>two longer lines</span>
    <span>flex item</span>
    <p>flex item</p>
</div>
```
#### tips
> The key thing to keep in mind is that once you set an element to be a flex container,
like the divs in Figure 12-1, it will only flex its immediate children, and not further descendants.

> flex容器只会影响它的直接（immediate ）子元素，而不会影响其它后代。

> Within a flex container, items line up on the main axis.The main axis can either be horizontal or vertical, so you can arrange items into columns or rows. The main axis takes on the directionality set via the writing mode.


### 2 flex Container
> The element on which  display: flex or  display: inline-flex  is applied becomes the flex container and generates a flex formatting context for its child elements.

> These children are lex items, whether they are DOM nodes, text nodes, or generated content. Absolutely positioned children of flex containers are also flex items, but each is sized and positioned as though it is the only flex item in the flex container.

#### flex-direction 
``` css
Values: row | row-reverse | column | column-reverse
Initial: value row
Applies to:  Flex containers
```
#### Wrapping Flex Lines (折叠flex行）
> If the flex items don’t all fit into the main axis of the flex container, by default the flex items will not wrap, nor will they necessarily resize. Rather, the flex items may shrink if allowed to do so via the flex item’s flex property  and/or the flex items may overflow the bounding container box.
``` css
Values: nowrap | wrap | wrap-reverse
Initial value: nowrap
Applies to: Flex containers
```
> The  flex-wrap property controls whether the flex container is limited to being a single-line container or is allowed to become multiline if needed.

#### Dening Flexible Flows
> The flex-flow property lets you define the directions of the main and cross axes, and whether the flex items can wrap to more than one line if needed.

``` css
Values: <flex-direction> ‖ <flex-wrap>
Initial value: row nowrap
Applies to: Flex containers
```
> The flex-flow shorthand property sets the flex-direction and flex-wrap properties to define the flex container’s wrapping and main and cross axes.

>As long as  display is set to  flex or  inline-flex, omitting  flex-flow, direction, and flex-wrap is the same as declaring any of the following three,
``` css
flex-flow: row;
flex-flow: nowrap;
flex-flow: row nowrap;
```
#### Understanding axes
> First: flex items are laid out along the main axis. Flex lines are added in the direction of the cross axis.

>  关键概念

> main axis : The axis along which content flows. In flexbox, this is the direction in which flex items are flowed.
main size: The total length of the content along the main axis.
main start: The end of the main axis from which content begins to flow.
main end: The end of the main axis toward which content flows, opposite the main start.
cross axis: The axis along which blocks are stacked. In flexbox, this is the direction in which new lines of flex items are placed, if flex wrapping is permitted.
cross size: The total length of the content along the cross axis.
cross start: The edge of the cross axis where blocks begin to be stacked.
cross end: The opposite edge of the cross axis from the cross start.

> 一图胜千言
![flex-direction](https://raw.githubusercontent.com/helloyangzhi/learn/master/flex-direction.png)

#### flex-wrap Continued
> The default value of  nowrap prevents wrapping, so the  cross-directions just discussed aren’t relevant when there is no chance of a second flex line.When additional lines are possible—when flex-wrap is set to wrap or wrap-reverse—those lines will be added in the cross direction. The first line is placed at the cross-start, with additional lines being added on the cross-end side.

### Flex Container
#### Justifying Content
>The  justify-content property enables us to direct how flex items are distributed along the main axis of the flex container within each flex line. It is applied to the flex container, not the individual flex items.

> 指定flex item在flex container中沿着main轴如何排列，该元素只能应用于flex container，不能应用于flex item.
``` css
Values: flex-start | flex-end | center | space-between | space-around |
space-evenly
Initial value: flex-start
Applies to: Flex containers
``` 
#### Aligning Items
> Whereas the justify-content defines how flex items are aligned along the flex container’s main axis, the align-items property defines how flex items are aligned along its flex line’s cross axis. 

> justify-content定义flex item沿着main轴排布，而align-item沿着纵轴排布。
``` css
Values: flex-start | flex-end | center | baseline | stretch
Initial value: stretch
Applies to: Flex containers
```

#### The align-self Property
``` css
Values: auto | flex-start | flex-end | center | baseline | stretch
Initial value: auto
Applies to: Flex items
```
> All the flex items have the  align-self’s default value of  auto set, meaning they inherit the alignment (in this case,  stretch) from the container’s  align-items property.

#### Aligning Content
> The align-content property aligns a flex container’s lines within a flex container that has extra space in the cross-axis direction, and dictates which direction will have overflow when there is not enough room to fit the flex lines.
``` css
Values: flex-start | flex-end | center | space-between | space-around | space-evenly | stretch
Initial value: stretch
Applies to: Multiline lex containers
```
> The  align-content property dictates how any extra cross-direction space in a flex container is distributed between and around flex lines. Although the values and concepts are the same, align-content is different from the previously discussed align-items property, which dictates flex item positioning within each flex line

> Think of align-content as similar to how justify-content aligns individual items along the main axis of the flex container, but it does it for flex lines with regard to the cross axis of the container. This property only applies to multiline flex containers, having no effect on non-wrapping and otherwise single-line flex containers.

``` css
.flex-container {
  display: flex;
  flex-flow: row wrap;
  align-items: flex-start;
  border: 1px dashed;
  height: 480px;
  background-image: url(banded.svg);
}
.flex-items {
    margin: 0;
    flow: 1;
}
```
![justify-content](https://raw.githubusercontent.com/helloyangzhi/learn/master/justify-content.png)

### Flex Items
#### What Are Flex Items?
> We create flex containers simply by adding a display: flex or display: inline-flex to an element that has child nodes. The children of those flex container are called  lex items—whether they’re child elements, non-empty text nodes between child elements, or generated content.

![flex-container-and-item](https://raw.githubusercontent.com/helloyangzhi/learn/master/flex-container-and-item.png)

#### Flex Item Features
> The margins of flex items do not collapse. The float and clear properties don’t have an effect on flex items, and do not take a flex item out of flow. In effect, float and clear are ignored when applied to flex items. (However, the float property can still affect box generation by influencing the  display  property’s computed value.)
``` css
aside {
  display: flex;
}
img {
  float: left;
}
<aside>
    <!-- this is a comment -->
    <h1>Header</h1>
    <img src="images/foo.jpg" alt="Foo Master">
    Some text
</aside>
```
``` css
aside {
  display: flex;
  align-items: center;
}
<aside>
    <!-- a comment -->
    <h1>Header</h1>
    <img src="images/foo.jpg" alt="foo master">
    Some text <a href="foo.html">with a link</a> and more text
</aside>
```

#### Absolute positioning
> While float will not actually float a flex item, setting position: absolute is a different story. The absolutely positioned children of flex containers, just like any other absolutely positioned element, are taken out of the flow of the document.

> More to the point, they do not participate in flex layout, and are not part of the document flow. However, they can be impacted by the styles set on the flex container, just as a child can be impacted by a parent element that isn’t a flex container. In addition to inheriting any inheritable properties, the flex container’s properties can affect the origin of the positioning

> The absolutely positioned child of a flex container is affected by both the justify-content value of the flex container, and its own align-self value, if there is one.

> The order property may not impact where the absolutely positioned flex container child is drawn, but it does impact the order of when it is drawn in relation to its siblings.

### The flex Property
> The defining aspect of flex layout is the ability to make the flex items “flex”: altering their width or height to fill the available space in the main dimension. A flex con‐tainer distributes free space to its items proportional to their flex grow factor, or shrinks them to prevent overflow proportional to their flex shrink factor.

> Declaring the  flex shorthand property on a flex item, or defining the individual properties that make up the shorthand, enables authors to define the grow and shrink factors. If there is excess space, you can tell the flex items to grow to fill that space. Or not. If there isn’t enough room to fit all the flex items within the flex container at their defined or default sizes, you can tell the flex items to shrink proportionally to fit into the space. Or not.

``` css
Values: [ <flex-grow> <flex-shrink>? ‖ <flex-basis> ] | none
Initial value: 0 1 auto
Applies to: Flex items (children of lex containers)
Percentages: Valid for flex-basis value only, relative to element’s parent’s inner main-axis size
```

#### The flex-grow Property
> The flex-grow property defines whether a flex item is allowed to grow when there is available space, and, if it is allowed to grow and there is available space, how much will it grow proportionally relative to the growth of other flex item siblings.

``` css
Values: <number>
Initial value: 0
Applies to: Flex items (children of lex containers)
```
> The value of flex-grow is always a number. Negative numbers are not valid. You can use non-integers if you like, just as long as they’re zero or greater. The value sets the flex growth factor, which determines how much the flex item will grow relative to the rest of the flex item siblings as the flex container’s free space is distributed.

> If there is any available space within the flex container, the space will be distributed proportionally among the children with a nonzero positive growth factor based on the various values of those growth factors.

> For example, assume a  750px wide horizontal flex container with three flex items, each set to width: 100px. That means there is a total of 300 pixels of space taken up by the flex items, leaving 450 pixels of “leftover” or available space (since 750 - 300 = 450).

![flex-grow](https://raw.githubusercontent.com/helloyangzhi/learn/master/flex-grow.png)

> In the second scenario in Figure 12-42, only one of the flex items (the third) has been given a growth factor. The declaration we gave it is flex-grow: 1, but it could be literally any positive number the browser can understand. In this case, with two items having no growth factor and the third having a growth factor, all of the available space is given to the flex item with a growth factor. Thus, the third flex item gets all 450 pixels of available space added to it, arriving at a final width of 550 pixels. The width: 100px applied to it elsewhere in the styles is overridden.

> In the third and fourth scenarios, the same flex item widths result despite the differ‐ ing flex growth factors. Let’s consider the third scenario, where the growth factors are 1, 1, and 3. The factors are all added together to get a total of 5. Each factor is then divided by that total to get a proportion. So here, the three values are each divided by five, yielding 0.2, 0.2, and 0.6.
These proportions are each multiplied by the available space to get the amount of growth. Thus:
>1. 450 px × 0.2 = 90 px
2. 450 px × 0.2 = 90 px
3. 450 px × 0.6 = 270 px

>Those are the growth portions added to each flex item’s starting width of 100 pixels. Thus, the final widths are 190 pixels, 190 pixels, and 370 pixels, respectively. The fourth scenario has the same result, because the proportions are the same. Imagine for a moment that we altered the growth factors to be 0.5, 1, and 1.5. Now the math works out such that the first flex item gets one-sixth of the available space, the second gets a third, and the third gets half. This results in the flex items’ final widths being 175, 250, and 425 pixels, respectively. Had we declared growth factors of 0.1, 0.1, and 0.3, or 25, 25, and 75, or really any combination of numbers with a 1:1:3 correspondence, the result would have been identical.

>As noted in “Minimum Widths” on page 613, if no width or flex basis is set, the flex basis defaults to auto, meaning each flex item basis is the width of its nonwrapped content. auto is a special value: it defaults to content unless the item has a width set on it, at which point the flex-basis becomes that width. The auto value is discussed in “Automatic Flex Basis” on page 635. Had we not set the width, in this example scenario, with our smallish font size, we would had more than 450 pixels of distributable space along the main axis.

>Now let’s consider a case where the flex items have different width values, as well as different growth factors. In Figure 12-43, in the second example, we have flex items that are 100 pixels, 250 pixels, and 100 pixels wide, with growth factors of 1, 1, and 3, respectively, in a container that is 750 pixels wide. This means we have 300 pixels of extra space to distribute among a total of 5 growth factors (since 750 - 450 = 300). Each growth factor is therefore 60 pixels (300 ÷ 5). This means the first and second flex items, with a flex-grow value of 1, will each grow by 60 pixels. The last flex item will grow by 180 pixels, since its flex-grow value is 3.

![flex-grow](https://raw.githubusercontent.com/helloyangzhi/learn/master/flex-grow2.png)

>To recap, the available space in the flex container, and the growth factors and final width of each flex item, are:
Available space: 750px - (100px + 250px + 100px) = 300px
Growth factors: 1 + 1 + 3 = 5
Width of each growth factor: 300px ÷ 5 = 60px
When flexed, the width of the flex items, based on their original width and growth
factors, become:
item1 = 100px + (1 × 60px) = 160px
item2 = 250px + (1 × 60px) = 310px
item3 = 100px + (3 × 60px) = 280px
which adds up to 750 pixels.

### The flex-shrink Property
> The <lex-shrink> portion of the flex shorthand property specifies the lex shrink factor. It can also be set via the flex-shrink property.

``` css
Values <number>
Initial value 1
Applies to Flex items (children of lex containers)
```
> The shrink factor determines how much a flex item will shrink relative to the rest of its flex-item siblings when there isn’t enough space for them all to fit, as defined by their content and other CSS properties. When omitted in the shorthand flex prop‐ erty value or when both flex and flex-shrink are omitted, the shrink factor defaults to 1. Like the growth factor, the value of flex-shrink is always a number. Negative numbers are not valid. You can use non-integer values if you like, just as long as they’re greater than zero.

> Basically, the shrink factor defines how “negative available space” is distributed when there isn’t enough room for the flex items and the flex container isn’t allowed to otherwise grow or wrap.

![flex-shrink](https://raw.githubusercontent.com/helloyangzhi/learn/master/flex-shrink1.png)
> except the flex items are set to width: 300px instead of 100 pixels. We still have a 750-pixels-wide flex container. The total width of the 3 items is 900 pixels, meaning the content starts out 150 pixels wider than the parent flex container. If the items are not allowed to shrink or wrap (see “Wrapping Flex Lines” on page  576), they will burst out from the fixed-size flex container. This is demonstrated in the first example in Figure 12-46: those items will not shrink because they have a zero shrink factor. Instead, they overflow the flex container.

> In the second example in  Figure 12-46, only the last flex item is set to be able to shrink. The last flex item is thus forced to do all the shrinking necessary to enable all the flex items to fit within the flex container. With 900 pixels worth of content needing to fit into our 750-pixel container, we have 150 pixels of negative available space. The 2 flex items with no shrink factor stay at 300 pixels wide. The third flex item, with a positive value for the shrink factor, shrinks 150 pixels, to end up 150 pixels wide. This enables the 3 items to fit within the container. (In this example the shrink factor was 1, but had it been 0.001 or 100 or 314159.65 or any other positive number the browser could understand, the result would be the same.)

> In the third example, we have positive shrink factors for all three flex items:
``` css
#example3 flex-item {
  flex-shrink: 1;
}
#example3 flex-item:last-child {
  flex-shrink: 3;
}
```
As this is the only one of the three flex shorthand properties we declared, this means the flex items will behave as if we had declared the following:
``` css
#example3 flex-item {
  flex: 0 1 auto; /* growth defaults to 0, basis to auto */
}
f#example3 flex-item:last-child {
  flex: 0 3 auto;
}
```
> If all items are allowed to shrink, as is the case here, the shrinking is distributed pro‐ portionally based on the shrink factors. This means the larger a flex item’s shrink factor, as compared to the shrink factors of its sibling flex items, the more the item will shrink in comparison. With a parent 750 pixels wide, and 3 flex items with a width of 300 pixels, there are 150 “negative space” pixels that need to be shaved off the flex items that are allowed to shrink (which is all of them in this example). With two flex items having a shrink factor of 1, and one flex item having a shrink factor of 3, we have a total of five shrink factors:
(2 × 1) + (1 × 3) = 5
With 5 shrink factors, and a total of 150 pixels needing to be shaved off all the flex items, each shrink factor is worth 30 pixels:
150px ÷ 5 = 30px
The default flex item size was 300 pixels, leading us to have 2 flex items with a width of 270 pixels each and the last flex item having a width of 210 pixels, which totals 750 pixels:
300px - (1 × 30px) = 270px
300px - (3 × 30px) = 210px
The following CSS produces the same outcome: while the numeric representation of the shrink factors are different, they are proportionally the same, so the flex item widths will be the same:
``` css
flex-item {
  flex: 1 0.25 auto;
}
flex-item:last-child {
  flex: 1 0.75 auto;
}
```
> Note that the flex items in these examples will shrink to 210, 210, and 270 pixels, respectively, as long as the content (like media objects or non-wrappable text) within each flex item is not wider than 210, 210, or 270 pixels, respectively. If the flex item contains content that cannot wrap or otherwise shrink in the main-dimension, the flex item will not shrink any further.

> Suppose that the first flex items contain a 300-pixels-wide image. That first flex item can not shrink, and other flex items can shrink, therefore it will not shrink, as if it had a null shrink factor. In this case, the first item would be 300 pixels, with the 150 pixels of negative space distributed proportionally based on the shrink factors of the second and third flex items.

> That being the case, we have 4 unimpeded shrink factors (one from the second flex item, and three from the third) for 150 pixels of negative space, with each shrink factor being worth 37.5 pixels. The flex items will end up 300, 262.5, and 187.5 pixels respectively, for a total of 750 pixels. The result is illustrated in Figure 12-47:
item1 = 300px - (0 × 37.5px) = 300.0px
item2 = 300px - (1 × 37.5px) = 262.5px
item3 = 300px - (3 × 37.5px) = 187.5px

![flex-shrink](https://raw.githubusercontent.com/helloyangzhi/learn/master/flex-shrink3.png)

> Had the image been 296 pixels wide, that first flex item would have been able to shrink by 4 pixels. The remaining 146 pixels of negative space would then be distributed the among the 4 remaining shrink, yielding 36.5 pixels per factor. The flex items would then be 296, 263.5, and 190.5 pixels wide, respectively.

> If all three flex items contained non-wrappable text or media 300 pixels or wider, the none of the three flex items would not shrink, appearing similar to the first example.

#### Proportional Shrinkage Based on Width and Shrink Factor
> The preceding code examples were fairly simple because all the flex items started with the same width. But what if the widths were different? What if the first and last flex items had a width of 250 pixels and the middle flex item had a width of 500 pixels

![flex-shrink](https://raw.githubusercontent.com/helloyangzhi/learn/master/flex-shrink4.png)

> Flex items shrink proportionally relative to both the shrink factor and the flex item’s width, with the width often being the width of the flex item’s content with no wrapping. In  Figure 12-48, we are trying to fit 1,000 pixels into a 750 pixels-width flex container. We have an excess of 250 pixels to be removed from 5 shrink factors.

> If this were a flex-grow situation, we would simply divide 250 pixels by 5, allocating 50 pixels per growth factor. If we were to shrink that way, we would get flex items 200, 550, and 100 pixels wide, respectively. But that’s not how shrinking actually works.

> Here, we have 250 pixels of negative space to proportionally distribute. To get the shrink factor proportions, we divide the negative space by the total of the flex items’ widths (more precisely, their lengths along the main axis) times their shrink factors:

![flex-shrink](https://raw.githubusercontent.com/helloyangzhi/learn/master/flex-shrink5.png)

> Using this equation, we find the shrink percentage:
= 250px ÷ ((250px × 1) + (500px × 1) + (250px × 3))
= 250px ÷ 1500px
= 0.166666667 (16.67%)
When we reduce each flex item by 16.67% times the value of flex-shrink, we end up with flex items that are reduced by:
item1 = 250px × (1 × 16.67%) = 41.67px
item2 = 500px × (1 × 16.67%) = 83.33px
item3 = 250px × (3 × 16.67%) = 125px
Each reduction is then subtracted from the starting sizes of 250, 500, and 250 pixels, respectively. We thus end up with flex items that are 208.33, 416.67, and 125 pixels wide.

#### Difering Bases
> With zero shrink factor, if both the width and flex basis of a flex item at auto, its content will not wrap, even when you think it should. Conversely, any positive shrink value enables content to wrap. Because shrinking is proportional based on shrink factor, if all the flex items have similar shrink factors, the content should wrap over a similar number of lines.

> In the three examples shown in Figure 12-49, the flex items do not have a declared width. Therefore, the width is based on the content, because width defaults to auto. The flex container has been made 520 pixels wide, instead of of our usual 750 pixels.
![flex-shrink](https://raw.githubusercontent.com/helloyangzhi/learn/master/flex-shrink6.png)

> Note that in the first example, where all the items have the same flex-shrink value, all content wraps over four lines. In the second example, the first flex item has a shrink factor half of value of the other flex items, so it wraps the content over (roughly) half the number of lines. This is the power of the shrink factor. In the third example, with no shrink factor, the text doesn’t wrap at all and the flex items overflow the container by quite a bit.

> Because the flex property’s shrink factor reduces the width of flex items proportionally, the number of lines of text in the flex items will grow or shrink as the width shrinks or grows, leading to similar height content within sibling flex items when the shrink factors are similar. 

> In the examples, take the contents of the flex items to be 280, 995, and 480 pixels, respectively—which is the width of the non-wrapping flex items in the third example (as measured by the developer tools, then rounded to make this example a little simpler). This means we have to fit 1,755 pixels of content into a 520 pixels-wide flex container by shrinking the flex items proportionally based on their shrink factor. This means we have 1,235 pixels of negative available space to proportionally distribute.

> In our first example, the flex items will end up with the same, or approximately the same, number of text lines. This is because flex items shrink proportionally, based on the width of their content. We didn’t declare any widths, and therefore can’t simply use an explicit element width as the basis for our calculations, as we did in the previous examples. Rather, we distribute the 1,235 pixels of negative space proportionally based on the widths of the content—280, 995, and 480 pixels, respectively. We determine 520 is 29.63% of 1,755. To determine the width of each flex item with a shrink factor of 1, we multiply the
content width of each flex item by 29.63%:

item1 = 280px × 29.63% = 83px
item2 = 995px × 29.63% = 295px
item3 = 480px × 29.63% = 142px

> With the default of  align-items: stretch (see  “Aligning Items” on page  596), a three-column layout will have three columns of equal height. By using a consistent shrink factor for all flex items, you can indicate that the actual content of these three flex items should be of approximately equal height—though, by doing this, the widths of those columns will not necessarily be uniform.

> In the second example in Figure 12-49, the flex items don’t all have the same shrink factor. The first flex item will, proportionally, shrink half as much as the others. We start with the same widths: 280, 995, and 480 pixels, respectively, but their shrink factors are 0.5, 1.0, and 1.0. As we know the widths of the content, the shrink factor (X) can be found mathematically
280px + 995px + 480px = 1615px
(0.5 × 280px) + (1 × 995px) + (1 × 480px) = 1235px
X = 1235px ÷ 1615px = 0.7647

> We can find the final widths now that we know the shrink factor. If the shrink factor is 76.47%, it means that  item2 and  item3 will be shrink by that amount, whereas item1 will shrink by 38.23% (because its flex-shrink value is half the others). The amount of shrinkage in each case is, rounded off to the nearest whole number:
item1 = 280px × 0.3823 = 107px
item2 = 995px × 0.7647 = 761px
item3 = 480px × 0.7647 = 367px
Thus, the final widths of the flex items is:
item1 = 280px - 107px = 173px
item2 = 995px - 761px = 234px
item3 = 480px - 367px = 113px
> The total combined widths of these 3 flex items is 520 pixels. Adding in varying shrink and growth factors makes it all a little less intuitive. That’s why you likely want to always declare the flex shorthand, preferably with a width or basis set for each flex item. If this doesn’t make sense yet, don’t worry; we’ll cover a few more examples of shrinking as we discuss flex-basis.