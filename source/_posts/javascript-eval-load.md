---
title: JS window eval and load script
date: 2014-06-04 09:46:48
tags:
---
1 .执行全局的 eval 代码
``` javascript
var globalEval = function globalEval(src) {
    if (window.execScript) {
        window.execScript(src);
        return;
    }
    var fn = function() {
        window.eval.call(window,src);
    };
    fn();
};
```
以下为腾讯的loadScript代码：
``` javascript
QQ.LoadScript = function(url, callback, charset){
	var script = document.createElement("script");
	script.type = "text/javascript";
	if(charset){
		script.setAttribute("charset",charset);
	}
	if (script.readyState){
		script.onreadystatechange = function(){   
		   if(script.readyState == "loaded" || script.readyState == "complete"){   
				script.onreadystatechange = null;
				callback();
				document.getElementsByTagName("head")[0].removeChild(this);
		   }
		};
	}
	else {
		script.onload = function(){
			callback();
			document.getElementsByTagName("head")[0].removeChild(this);
		};
	}
	script.src = url;
	document.getElementsByTagName("head")[0].appendChild(script);
}
```
以下为腾讯判断ie 7与 ie 8的办法:
``` javascript
(function($){
  var winWidth = $(window).width();
  var winHeight = $(document).height();
  var shadowWidth = (winWidth - 1002)/2;
  $('#followMe').height( winHeight );
  var dm=document.documentMode;
  var ie=!-[1,];
  var ie7=ie&&dm==7;
  var ie8=ie&&dm==8;
  if( ie7||ie8 ){
      $('#fShaowL').width(shadowWidth+1);
  }else{
      $('#fShaowL').width(shadowWidth);
  }
  $('#fShaowR').width(shadowWidth);
  $('#followBottom').height( winHeight - 1572 );
  var img = new Image();
  img.src= 'http://mat1.gtimg.com/www/images/qq2012/guide_k_01.png';
})(jQuery);
```
以下为腾讯对天气的处理办法，不失为一种对复杂数据的处理办法：
``` javascript
var weatherCityMap ={
  "weatherCityWind":{
    "0":{
      "Dir":"", 
      "Power":"微风"
    },
    "1":{
      "Dir":"东北风",
      "Power":"3-4级"
    },
    "2":{
      "Dir":"东风",
      "Power":"4-5级"
    },
    "3":{
      "Dir":"东南风",
      "Power":"5-6级"
    },
    "4":{
      "Dir":"南风",
      "Power":"6-7级"
    },
    "5":{
      "Dir":"西南风",
      "Power":"7-8级"
    },
    "6":{
      "Dir":"西风",
      "Power":"8-9级"
    },
    "7":{
      "Dir":"西北风",
      "Power":"9-10级"
    },
    "8":{
      "Dir":"北风",
      "Power":"10-11级"
    },
    "9":{
      "Dir":"旋转不定",
      "Power":"11-12级"
    }
  },
  "weatherCityIcon":{
    "default":{
      "big":"http://mat1.gtimg.com/weather/weatherIco/midImg/",
      "mid":"http://mat1.gtimg.com/www/images/qq2012/weather/icon35/",
      "small":"http://mat1.gtimg.com/www/images/qq2012/weather/"
    },
    "00":{
      "icon":"m0.png",
      "weatherTxt":"晴"
    },
    "01":{
      "icon":"m1.png",
      "weatherTxt":"多云"
    },
    "02":{
      "icon":"m2.png",
      "weatherTxt":"阴"
    },
    "03":{
      "icon":"m3.png",
      "weatherTxt":"阵雨"
    },
    "04":{
      "icon":"m4.png",
      "weatherTxt":"雷阵雨"
    },
    "05":{
      "icon":"m4.png",
      "weatherTxt":"雷阵雨并伴有冰雹"
    },
    "06":{
      "icon":"m6.png",
      "weatherTxt":"雨夹雪"
    },
    "07":{
      "icon":"m7.png",
      "weatherTxt":"小雨"
    },
    "08":{
      "icon":"m8.png",
      "weatherTxt":"中雨"
    },
    "09":{
      "icon":"m9.png",
      "weatherTxt":"大雨"
    },
    "10":{
      "icon":"m10.png",
      "weatherTxt":"暴雨"
    },
    "11":{
      "icon":"m17.png",
      "weatherTxt":"大暴雪"
    },
    "12":{
      "icon":"m17.png",
      "weatherTxt":"特大暴雪"
    },
    "13":{
      "icon":"m14.png",
      "weatherTxt":"阵雪"
    },
    "14":{
      "icon":"m14.png",
      "weatherTxt":"小雪"
    },
    "15":{
      "icon":"m16.png",
      "weatherTxt":"中雪"
    },
    "16":{
      "icon":"m16.png",
      "weatherTxt":"大雪"
    },
    "17":{
      "icon":"m17.png",
      "weatherTxt":"暴雪"
    },
    "18":{
      "icon":"m18.png",
      "weatherTxt":"雾"
    },
    "19":{
      "icon":"m6.png",
      "weatherTxt":"冻雨"
    },
    "20":{
      "icon":"m20.png",
      "weatherTxt":"沙尘暴"
    },
    "21":{
      "icon":"m8.png",
      "weatherTxt":"小雨-中雨"
    },
    "22":{
      "icon":"m9.png",
      "weatherTxt":"中雨-大雨"
    },
    "23":{
      "icon":"m10.png",
      "weatherTxt":"大雨-暴雨"
    },
    "24":{
      "icon":"m10.png",
      "weatherTxt":"暴雨-大暴雨"
    },
    "25":{
      "icon":"m10.png",
      "weatherTxt":"大暴雨-特大暴雨"
    },
    "26":{
      "icon":"m16.png",
      "weatherTxt":"小雪-中雪"
    },
    "27":{
      "icon":"m16.png",
      "weatherTxt":"中雪-大雪"
    },
    "28":{
      "icon":"m17.png",
      "weatherTxt":"大雪-暴雪"
    },
    "29":{
      "icon":"m29.png",
      "weatherTxt":"浮尘"
    },
    "30":{
      "icon":"m29.png",
      "weatherTxt":"扬沙"
    },
    "31":{
      "icon":"m20.png",
      "weatherTxt":"强沙尘暴"
    },
    "32":{
      "icon":"m9.png",
      "weatherTxt":"飑"
    },
    "33":{
      "icon":"m33.png",
      "weatherTxt":"龙卷风"
    },
    "34":{
      "icon":"m14.png",
      "weatherTxt":"弱高吹雪"
    },
    "35":{
      "icon":"m18.png",
      "weatherTxt":"轻雾"
    },
    "53":{
      "icon":"m29.png",
      "weatherTxt":"霾"
    }
  },
  "living":{
    'cl': [{ 'type': '', 'info': '' }, { 'type': '适宜', 'info': '天气晴朗，空气清新，是您晨练的大好时机。' }, { 'type': '较适宜', 'info': '较适宜晨练，某些气象条件会对晨练产生一定影响，但影响不大。' }, { 'type': '较不宜', 'info': '某些气象因素对晨练造成不利影响，较不宜晨练。' }, { 'type': '不适宜', 'info': '气象因素非常不利于室外锻炼，请尽量避免户外晨练。'}],
    'cy': [{ 'type': '', 'info': '' }, { 'type': '炎热', 'info': '薄型T恤衫。' }, { 'type': '热舒适', 'info': '短套装、T恤夏季服装。' }, { 'type': '舒适', 'info': '长袖服装。' }, { 'type': '凉舒适', 'info': '薄型套装等春秋过渡装。'},{ 'type': '温凉', 'info': '夹衣或西服套装加薄羊毛衫。' }, { 'type': '凉', 'info': '厚外套加毛衣等春秋服装。' }, { 'type': '冷', 'info': '棉衣加羊毛衫等冬季服装。' }, { 'type':'寒冷','info': '厚羽绒服等隆冬服装。'}],
    'gm': [{ 'type': '', 'info': '' }, { 'type': '少发', 'info': '各项气象条件适宜，发生感冒机率较低。' }, { 'type': '较易发', 'info': '较易发生感冒，体质较弱的朋友请注意适当防护。' }, { 'type': '易发', 'info': '发生感冒机率较大，请加强自我防护避免感冒。'}, { 'type': '极易发', 'info': '极易发生感冒，请特别注意增加衣服保暖防寒避免感冒。'}],
    'xc': [{ 'type': '', 'info': '' }, { 'type': '适宜', 'info': '适宜洗车，未来持续两天无雨天气较好，适合擦洗汽车。' }, { 'type': '较适宜', 'info': '较适宜洗车，未来一天无雨，风力较小，较适合擦洗汽车。' }, { 'type': '较不宜', 'info': '较不宜洗车，擦洗一新的汽车可能会蒙上污垢。'}, { 'type': '不宜', 'info': '不宜洗车，路上的泥水可能会再次弄脏您的爱车。'}],
    'zs': [{ 'type': '无', 'info': '温度不高，其他各项气象条件适宜，中暑机率极低。' }, { 'type': '少发', 'info': '气温偏高，有可能中暑，体质较弱的朋友请注意防暑降温，避免长时间在日光下暴晒或在高温环境中工作。' }, { 'type': '较易', 'info': '气温较高，较易中暑，体弱者请避免长时间在日光下暴晒或在高温环境中工作。' }, { 'type': '容易', 'info': '气温很高，热气逼人，容易中暑，请注意防暑降温，避免长时间在日光下暴晒或在高温环境中工作。'}, { 'type': '极易', 'info': '气温极高，热浪滚滚，极易中暑，请注意防暑降温，避免在高温环境中工作。'}],
    'zwx': [{ 'type': '', 'info': '' }, { 'type': '最弱', 'info': '属弱紫外线辐射天气，无需特别防护。若长期在户外，建议涂擦SPF在8-12之间的防晒护肤品。' }, { 'type': '弱', 'info': '紫外线强度较弱，建议出门前涂擦SPF在12-15之间、PA+的防晒护肤品。' }, { 'type': '中等', 'info': '属中等强度紫外线辐射天气，外出时建议涂擦SPF高于15、PA+的防晒护肤品，戴帽子、太阳镜。'}, { 'type': '强', 'info': '紫外线辐射强，建议涂擦SPF20左右、PA++的防晒护肤品。避免在10点至14点暴露于日光下。'}, { 'type': '很强', 'info': '紫外线辐射极强，建议涂擦SPF20以上、PA++的防晒护肤品，尽量避免暴露于日光下。'}],
    'ls': [{ 'type': '', 'info': '' }, { 'type': '极适宜', 'info': '极适宜晾晒，请抓紧时机晾晒。' }, { 'type': '适宜', 'info': '适宜晾晒，赶紧把久未见阳光的衣物搬出来吸收一下太阳的味道吧！' }, { 'type': '基本适宜', 'info': '午后温暖的阳光仍能满足你驱潮消霉杀菌的晾晒需求。'}, { 'type': '不太适宜', 'info': '气象要素对晾晒有影响，不太适宜晾晒。若非晾晒不可，请尽量选择通风的地点。'}, { 'type': '不适宜', 'info': '不适宜晾晒。如果非晾晒不可，请在室内进行并关牢门窗。'}],
    'kqwr': [{ 'type': '', 'info': '' }, { 'type': '优', 'info': '非常有利于空气污染物稀释、扩散和清除，可在室外正常活动。' }, { 'type': '良', 'info': '有利于空气污染物稀释、扩散和清除，可在室外正常活动。' }, { 'type': '中', 'info': '对空气污染物稀释、扩散和清除无明显影响，易感人群应适当减少室外活动时间。'}, { 'type': '较差', 'info': '较不利于空气污染物稀释、扩散和清除，请适当减少室外活动时间。'}, { 'type': '差', 'info': '不利于空气污染物稀释、扩散和清除，请尽量避免在室外长时间活动。'}],
    'ly': [{ 'type': '', 'info': '' }, { 'type': '适宜', 'info': '明媚阳和微风伴您一路同行，适宜旅游。' }, { 'type': '较适宜', 'info': '虽然会多云或有风，但仍是出行游玩的好时机，较适宜旅游。' }, { 'type': '一般', 'info': '可能出现阴天、大风、阵雨或沙尘等天气，将会给出行带来一些不便，旅游指数一般。'}, { 'type': '较不宜', 'info': '可能出现雨雪，强风，大雾等天气，人体在户外会感觉不舒适，将会给出行带来很多不便，较不适宜出游。'}, { 'type': '不适宜', 'info': '可能出现较大的雨雪，强风，冻雨，沙尘暴等天气，对出行造成一定困难，不适宜旅游。'}],
    'kt': [{ 'type': '', 'info': '' },{ 'type': '长时间开启', 'info': '闷热，您需长时间开启制冷空调来降温除湿。' }, { 'type': '部分时间开启', 'info': '天气热，建议您在适当的时候开启制冷空调来降低温度。' }, { 'type': '少部分时间', 'info': '中午的时候您将会感到有点热，因此建议在午后较热时开启制冷空调。' }, { 'type': '较少开启', 'info': '您将感到很舒适，一般不需要开启空调。'}, { 'type': '开启制暖空调', 'info': '冷,适当开启制暖空调调节室内温度。'}],
    'dy': [{ 'type': '', 'info': '' }, { 'type': '适宜', 'info': '非常适宜垂钓，风和日丽的天气将陪伴你度过愉快的垂钓时光。' }, { 'type': '较适宜', 'info': '较适合垂钓，气象条件对垂钓会产生一定影响，但影响不大。' }, { 'type': '不适宜', 'info': '天气不好，不适合垂钓，请选择别的娱乐方式。'}],
    'fs': [{ 'type': '', 'info': '' },{ 'type': '弱', 'info': '长期在户外，建议涂擦SPF在8-12之间的防晒护肤品。' }, { 'type': '较弱', 'info': '建议涂擦SPF在12-15之间，PA+的防晒护肤品。' }, { 'type': '中等', 'info': '建议涂擦SPF指数高于15,PA+的防晒护肤品。' }, { 'type': '强', 'info': '外出时应加强防护，建议涂擦SPF在15—20之间PA++的防晒护肤品。'}, { 'type': '极强', 'info': '外出时应特别加强防护，建议涂擦SPF20以上，PA++的防晒护肤品，并随时补涂。'}],
    'gj': [{ 'type': '适宜', 'info': '天气条件很适合逛街，可以尽情享受逛街的乐趣。' }, { 'type': '较适宜', 'info': '气象要素对逛街有一定影响，比较适宜逛街。' }, { 'type': '较不宜', 'info': '天气条件不适宜逛街，应尽量避免外出逛街。' }, { 'type': '不适宜', 'info': '气象条件不适合逛街，请尽量选择室内活动。'}],
    'hc': [{ 'type': '', 'info': '' }, { 'type': '适宜', 'info': '天气晴朗，温度适宜，非常适合划船或嬉玩各种水上运动。' }, { 'type': '较适宜', 'info': '较适宜划船，有些气象因素对划船会产生些影响，但影响不大。' }, { 'type': '不适宜', 'info': '不适宜划船，建议选择别的娱乐方式或采取必要措施。'}],
    'jt': [{ 'type': '', 'info': '' },{ 'type': '良好', 'info': '交通气象条件良好，车辆可以正常行驶。' }, { 'type': '较好', 'info': '交通气象条件较好，但不适宜高速行驶，司机应更加集中注意力，保持车距。' }, { 'type': '一般', 'info': '交通气象条件一般，刹车距离延长，事故易发期，注意车距，务必小心驾驶。' }, { 'type': '较差', 'info': '交通气象条件较差，事故高发期，车辆应低速行驶。'}, { 'type': '很差', 'info': '交通气象条件很差，车辆行驶缓慢，尽量减少出行。'}],
    'ys': [{ 'type': '', 'info': '' }, { 'type': '不带伞', 'info': '降水概率很低，因此您在出门的时候无须带雨伞。' }, { 'type': '带伞', 'info': '将有阵雨或阵雪，如果您要短时间外出的话可不必带雨伞。' }, { 'type': '带伞', 'info': '有小雨或小雪或中雪，在短时间外出可收起雨伞，但最好还是带上雨伞。'}, { 'type': '带伞', 'info': '会有较大的雨雪天气，您在外出的时候一定要带雨伞，以免被雨水淋湿。'}],
    'mf': [{ 'type': '', 'info': '' }, { 'type': '极适宜', 'info': '各项气象条件都适宜美发，这为您的头发创造一个健康、洁净的生长环境。' }, { 'type': '适宜', 'info': '有某项气象因素会影响您的秀发生长，请注意保养。 ' }, { 'type': '一般', 'info': '气象条件对美发有一定影响，注意保养您的美发，细心呵护！'}],
    'ysh': [{ 'type': '', 'info': '' }, { 'type': '适宜', 'info': '天气晴朗，你可以尽情外出享受夜生活的乐趣，不用担心天气会来捣乱。' }, { 'type': '较适宜', 'info': '虽然有风或有雨雪天气出现，但只要提前有所准备，您仍然可以享受夜生活的乐趣。' }, { 'type': '较不宜', 'info': '天气会使人体在户外感觉不舒适，建议夜生活最好在室内进行。'}],
    'ffz': [{ 'type': '', 'info': '' }, { 'type': '适宜', 'info': '这种天气去放风筝既可以舒展筋骨，又可放松身心。' }, { 'type': '较适宜', 'info': '选择合适的地点，还是较适宜放风筝的。' }, { 'type': '不宜', 'info': '气象条件不适宜放风筝。'}],
    'hz': [{ 'type': '', 'info': '' }, { 'type': '保湿防龟裂', 'info': '天气寒冷，多补水，选用滋润保湿型化妆品，使用润唇膏。' }, { 'type': '保湿', 'info': '皮肤易缺水，用保湿型霜类化妆品，使用润唇膏。' }, { 'type': '控油', 'info': '建议用露质面霜打底，水质无油粉底霜，透明粉饼，粉质胭脂。'}, { 'type': '防晒', 'info': '天气炎热，易出汗，建议使用防脱水防晒指数高的化妆品，经常补粉。'}],
    'fh': [{ 'type': '', 'info': '' }, { 'type': '凉', 'info': '室外活动注意适当增减衣物。' }, { 'type': '冷', 'info': '室外活动要穿厚实一点，年老体弱者要适当注意保暖。' }, { 'type': '寒冷', 'info': '室外活动要注意保暖，可戴手套与帽子。' }, { 'type': '非常寒冷', 'info': '室外活动注意保暖防寒，可戴厚手套和帽子，年老体弱者避免长时间外出。' }, { 'type': '严寒', 'info': '着羽绒服、皮大衣仍感到寒冷，室外活动须戴厚棉、皮手套和帽子。' }, { 'type': '冰冻严寒', 'info': '极易造成裸露皮肤冻伤，尽量避免野外作业和外出。' }, { 'type': '', 'info': '' }, { 'type': '微凉', 'info': '温度未达到风寒所需的低温，稍作防寒准备即可。'}],
    'xq': [{ 'type': '', 'info': '' }, { 'type': '好', 'info': '天气晴朗，阳光灿烂，空气温润，和风飘飘，美好的天气会带来一天接踵而来的好心情。' }, { 'type': '较好', 'info': '温度舒适，您会觉得精神振奋，意气风发，心情舒畅。' }, { 'type': '较差', 'info': '天气阴沉或有雾，会感觉莫名的压抑，情绪低落。'}, { 'type': '差', 'info': '天气阴沉有雨或有沙尘，闷热潮湿的空气会让人感到胸闷，心情糟糕。'}],
    'yd': [{ 'type': '', 'info': '' }, { 'type': '适宜', 'info': '天气较好，且紫外线辐射不强，适宜户外运动。' }, { 'type': '较适宜', 'info': '较适宜在户内低强度运动，户外运动需防晒避风。' }, { 'type': '较不宜', 'info': '受大风，气压，强紫外线，过高或过低的气温等天气影响，较不宜运动'}],
    'yh': [{ 'type': '', 'info': '' }, { 'type': '适宜', 'info': '天气晴朗，风和日丽，适宜与情人约会。' }, { 'type': '较适宜', 'info': '天气不会有太大的影响，你仍然可以有一个愉快的约会。' }, { 'type': '较不适宜', 'info': '会有影响人体舒适的不好天气出现，室外约会可能会让恋人们受些苦，最好在温暖的室内促膝谈心'}, { 'type': '不适宜', 'info': '天气会对人体有较大影响，外出约会还可能会败兴而归，男士请别约美眉逛街。'}]
  }
}
```


