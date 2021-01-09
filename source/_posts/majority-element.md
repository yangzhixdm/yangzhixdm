---
title: Majority Element
date: 2016-09-20 13:05:47
tags:
description: Majority Element/ 一种简单的算法
---
# Majority Element

Given an array of size n, find the majority element. The majority element is the element that appears more than ⌊ n/2 ⌋ times.

You may assume that the array is non-empty and the majority element always exist in the array.


```javascript
/**
 * @param {number[]} nums
 * @return {number}
 */
var majorityElement = function(nums) {
    var ret = [];
    var max = -1;
    var key = -1;
    for(var i = 0 ; i< nums.length; i ++){
        if(ret[nums[i]]){
            ret[nums[i]] = ret[nums[i]] + 1;
        }else{
            ret[nums[i]] = 1;
        }
        if(ret[nums[i]] > max){
            max = ret[nums[i]];
            key = nums[i];
        }
    }
    if(max - (nums.length - max )){
        return key;
    }else{
        return -1;
    }
};
```