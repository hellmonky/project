<%@ page language="java" import="java.util.*" contentType="text/html;charset=UTF-8" pageEncoding="UTF-8"%>
<%
String path = request.getContextPath();
String basePath = request.getScheme()+"://"+request.getServerName()+":"+request.getServerPort()+path+"/";
%>
<%@ taglib prefix="s" uri="/struts-tags" %>
<%@ taglib prefix="shiro" uri="http://shiro.apache.org/tags" %>

<%@ page import="iscas.entity.Profile"%>

<!DOCTYPE html>
<html lang="zh-cn">
<head>
    <meta charset="utf-8">
    <base href="<%=basePath%>">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Search</title>

    <!-- Bootstrap -->
    <link href="css/bootstrap.min.css" rel="stylesheet">
	<link href="css/main.css" rel="stylesheet">
	
    <!-- HTML5 Shim and Respond.js IE8 support of HTML5 elements and media queries -->
    <!-- WARNING: Respond.js doesn't work if you view the page via file:// -->
    <!--[if lt IE 9]>
      <script src="http://cdn.bootcss.com/html5shiv/3.7.2/html5shiv.min.js"></script>
      <script src="http://cdn.bootcss.com/respond.js/1.4.2/respond.min.js"></script>
    <![endif]-->
	<style type='text/css'>
	
	</style>
</head>

<body style="background-color:#F0F0F0">

<div class="container-fluid">

	<div class="row" style="color:white;background-color:#4791D2;" id="table">
		<div class="col-md-2 col-md-offset-1" style="padding:20px;">
			<span class="glyphicon glyphicon-home">
				<a href="index.html" style="color:white;">回到首页</a>
			</span>
			
			<span class="glyphicon glyphicon-user">
				<a target="_blank" href="compare.html?name=<s:property value="#request.profile.name"/>" style="color:white;">人物对比</a>
			</span>
			
			<span class="glyphicon glyphicon-file">
				<a target="_blank" href="log.html?name=<s:property value="#request.profile.name"/>&xmlpath=<s:property value="#request.profile.xmlPath"/>&id=<s:property value="#request.profile.id"/>" style="color:white;">文件日志</a>
			</span>
		</div>
		
		<div class="col-md-5" style="padding:20px;">	
	    	<form action="searchByName.action" role="form" method="post">
	    		<div class="input-group">
					<label class="sr-only" for="exampleInputName">Profile Name</label>	
	    			<input type="text" class="form-control typeahead"  placeholder="请输入姓名" name="name" id="name">    			
	    			<span class="input-group-btn">
				        <button class="btn btn-primary" type="submit">搜索</button>
				    </span>
	    		</div>
			</form>			
		</div>
		
		<div class="col-md-2" style="padding:20px;">
       		<h5 id="digitaltime"></h5>
       	</div>
	</div>
	<br>
	<div class="row">
		<div class="basic_info">
		<div class="col-md-6">
			<div class="row">
				<div class="col-md-4">
					<a href="#" class="thumbnail">
						<img src="<s:url action='fileDisplay.action?id=%{#request.profile.id}'/>" class="img-responsive" style="height:200px" alt="Responsive image"/>
				    </a>
				</div>
				<div class="col-md-8">
					<div class="panel panel-default">
						<p hidden id="profileId"><s:property value="#request.profile.id"/></p>
	  					<div class="panel-heading">
	  						<span class="glyphicon glyphicon-user"></span>&nbsp;&nbsp;基本信息
	  						
	  						<shiro:hasRole name="admin">
		  						<!-- only admin user could do edit and delete action -->
		  						<!-- add one new edit person info button -->
		  						<button class="btn btn-info btn-sm" style="margin-left:100px" onclick="window.location.href='edit.html?id=<s:property value="#request.profile.id"/>'">
									<span class="glyphicon glyphicon-edit"></span>&nbsp;&nbsp;编辑
								</button>
								
		  						<!-- add one new delete person button -->
								<button class="btn btn-danger btn-sm" data-toggle="modal" data-target="#myModal">
									<span class="glyphicon glyphicon-trash"></span>&nbsp;&nbsp;删除
								</button>
							</shiro:hasRole>			
	  					</div>
	  					
						<table class="table table-hover">
							<tr>
								<td>姓名</td>
								<td id="profileName"><s:property value="#request.profile.name"/></td>
								<td>性别</td>
								<td><s:property value="#request.profile.gender"/></td>
							</tr>
							<tr>
								<td>民族</td>
								<td><s:property value="#request.profile.nation"/></td>
								<td>籍贯</td>
								<td><s:property value="#request.profile.birthProvince"/>&nbsp<s:property value="#request.profile.birthCity"/></td>
							</tr>
							<tr>
								<td>年龄</td>
								<td><s:property value="#request.profile.age"/></td>
								<td>出生日期</td>
								<td><s:property value="#request.profile.birthDate"/></td>
							</tr>
							<tr>
								<td>入党日期</td>
								<td><s:property value="#request.profile.partyDate"/></td>
								<td>工作日期</td>
								<td><s:property value="#request.profile.workDate"/></td>
							</tr>
							<tr>
								<td>现任职务</td>
								<td colspan="3">
									<s:iterator value="#request.profile.latestOfficeRecord.tupleList" id='tuple'>				
										<s:property value='#tuple.content'/>
									</s:iterator>
								</td>
							</tr>
							<tr>
								<td>档案级别</td>
								<td id="currentLevel"><s:property value="#request.profile.level"/></td>
								
								<shiro:lacksRole name="admin">
								    <td></td>
								    <td></td>
								</shiro:lacksRole>
								
								<shiro:hasRole name="admin">
								<td>调整级别</td>
								<td>
									<!-- rate -->
									<div class="btn-group">
									  <button type="button" class="btn btn-default btn-sm" onclick="setLevel(1)">1</button>
									  <button type="button" class="btn btn-default btn-sm" onclick="setLevel(2)">2</button>
									  <button type="button" class="btn btn-default btn-sm" onclick="setLevel(3)">3</button>
									</div>
								</td>
								</shiro:hasRole>
							</tr>
							<tr>
								<td>更新时间</td>
								<td id="updateTime"></td>
								
								<td></td>
								<td></td>
							</tr>
						</table>
					</div>
					
				</div>
				
				<div class="col-md-12">	
					<div class="panel panel-default">
						<div class="panel-heading">
							<h4 class="panel-title"><span class="glyphicon glyphicon-file"></span>&nbsp;&nbsp;详细内容</h4>
						</div>
						<div  style="height:200px; overflow:auto">
							<div class="panel-body" id="rawText">			
							</div>
						</div>
					</div>
				</div>
				
			</div>
		</div>
		</div>
		
		<!-- 选项卡式显示多个类型的人物关系图 -->
		<div class="col-md-4">
			<div style="position: relative; margin-left: 0px;" class="author-vis-matrix">
			
			<!-- Nav tabs -->
			<ul id="myTab" class="nav nav-pills" role="tablist" style="background-color:#F0F0F0">			
				<li class="active"><a href="#all" role="tab" data-toggle="tab">综合</a></li>
			
			  	<li><a href="#location" role="tab" data-toggle="tab">同乡</a></li>
			  	
			  	<li class="dropdown"><a href="#" id="myTabDrop1" class="dropdown-toggle" data-toggle="dropdown">同事<span class="caret"></span></a>
					<ul class="dropdown-menu" role="menu" aria-labelledby="myTabDrop1">
						<li class=""><a href="#colleague" tabindex="-1" role="tab" data-toggle="tab">按时间显示</a></li>
						<li class=""><a href="#colleague2" tabindex="-1" role="tab" data-toggle="tab">按级别显示</a></li>
					</ul>
				</li>
				
				<li class="dropdown"><a href="#" id="myTabDrop2" class="dropdown-toggle" data-toggle="dropdown">同学<span class="caret"></span></a>
					<ul class="dropdown-menu" role="menu" aria-labelledby="myTabDrop2">
						<li class=""><a href="#currentClassMate" tabindex="-1" role="tab" data-toggle="tab">当前学习经历</a></li>
						<li class=""><a href="#previousClassMate" tabindex="-1" role="tab" data-toggle="tab">历史学习经历</a></li>
					</ul>
				</li>
				
				<li class="text pull-right">
					<a shape="rect" id="visToggleSizeButton" onclick="toggleVis();" href="javascript:void(0);" class="pull-right">
						<span id="visToggleSizeIcon" class="glyphicon glyphicon-resize-full"></span>
					</a>
				</li>				
			</ul>

			<!-- Tab panes -->
			<div class="tab-content">
				<div class="tab-pane fade active in" id="all">
					<div id="allRelation" class="relation-chart"></div>
				</div>
				<div class="tab-pane fade" id="location">
					<div id="locationRelation" class="relation-chart"></div>
				</div>
				<div class="tab-pane fade" id="colleague">
					<div id="colleagueRelation" class="relation-chart"></div>
				</div>
				<div class="tab-pane fade" id="colleague2">
					<div id="colleagueRelation2" class="relation-chart"></div>
				</div>
				<div class="tab-pane fade" id="currentClassMate">
					<div id="currentClassMateRelation" class="relation-chart"></div>
				</div>
				<div class="tab-pane fade" id="previousClassMate">
					<div id="previousClassMateRelation" class="relation-chart"></div>
				</div>
			</div>
	
			<div class="row">
				<div class="col-sm-3">
					<div class="input-group input-group-sm">
						<span class="input-group-addon" style="color:white;background-color:#4791D2">同乡</span>
						<input type="text" class="form-control" placeholder="权重" name="factor" value="0.33" style="width:50px">
					</div>
				</div>
				<div class="col-sm-3">
					<div class="input-group input-group-sm">
						<span class="input-group-addon" style="color:white;background-color:#4791D2">同事</span>
						<input type="text" class="form-control" placeholder="权重" name="factor" value="0.33" style="width:50px">
					</div>
				</div>
				<div class="col-sm-3">
					<div class="input-group input-group-sm">
						<span class="input-group-addon" style="color:white;background-color:#4791D2">同学</span>
						<input type="text" class="form-control" placeholder="权重" name="factor" value="0.33" style="width:50px">
					</div>
				</div>
				<div class="col-sm-3">
					<button type="button" id="applyButton" class="btn btn-primary btn-sm">
					  	应用
					</button>
				</div>
			</div>
			
			</div>
		</div>

	</div>
	
	<div class="row">
		<div class="col-md-6">
			<div id="office" style="height:300px;border:1px solid #ccc;padding:10px;"></div>
		</div>

		<div class="col-md-6">
			<div id="map" style="height:300px;width:500px;border:1px solid #ccc;padding:10px;"></div>
		</div>
	</div>

</div>

<!-- Modal删除确认模态对话框 -->
<div class="modal fade" id="myModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true" style="margin-top:100px">
  <div class="modal-dialog">
    <div class="modal-content">
      <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal"><span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button>
        <h4 class="modal-title" id="myModalLabel">确认删除该人简历?</h4>
      </div>
      <div class="modal-body">
        <s:property value="#request.profile.name"/>
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-default" data-dismiss="modal">取消</button>
        <button type="button" class="btn btn-primary" onclick="deleteProfile()">确定</button>
      </div>
    </div>
  </div>
</div>

<script src="js/jquery.min.js"></script>
<script src="js/bootstrap.min.js"></script>
<script src="js/echarts-all.js"></script>
<script src="js/digitaltime.js"></script>
<script src="js/typeahead.bundle.min.js"></script>
<script src="js/input-autocomplete.js"></script>
<script src="js/moment-with-locales.min.js"></script>

<script>
	var name = document.getElementById("profileName").innerHTML;
	var id = document.getElementById("profileId").innerHTML;
	console.log("该人物的id：" + id);
	
	var inputsFactor = document.getElementsByName('factor');
	var len = inputsFactor.length;
	for(var i = 0; i < len; i++){
		inputsFactor[i].onkeyup = function() {
			otherSum(this.value, this);
		};
	}
	
	function otherSum(val,obj) {
		
		if (inputsFactor[0] == obj) {
			if (val > 1 || val < 0) {
		        alert('对不起，请输入0-1的小数');
		        $.each(inputsFactor, function(){
		        	this.value = 0.33;
				})
  			} else
				inputsFactor[1].value = inputsFactor[2].value = ((1.0 - parseFloat(val)) / 2).toFixed(2);
		} else if (inputsFactor[1] == obj) {
			if (val > 1 || val < 0) {
		        alert('对不起，请输入0-1的小数');
		        $.each(inputsFactor, function(){
					this.value = 0.33;
				})
  			}
			inputsFactor[2].value = (1.0 - parseFloat(val) - parseFloat(inputsFactor[0].value)).toFixed(2);
			
			if (inputsFactor[2].value > 1 || inputsFactor[2].value < 0) {
		        $.each(inputsFactor, function(){
					this.value = 0.33;
				})
			}
			
		} else if (inputsFactor[2] == obj) {
			if (val > 1 || val < 0) {
		        alert('对不起，请输入0-1的小数');
		        $.each(inputsFactor, function(){
					this.value = 0.33;
				})
  			}
			inputsFactor[1].value = (1.0 - parseFloat(val) - parseFloat(inputsFactor[0].value)).toFixed(2);
			
			if (inputsFactor[1].value > 1 || inputsFactor[1].value < 0) {
		        $.each(inputsFactor, function(){
					this.value = 0.33;
				})
			}
		}
	}
	
	
	//点击综合选项卡后显示三个输入框和应用按钮
	$('#myTab a').click(function (e) {
		e.preventDefault()
		$(this).tab('show')
		if (e.target.innerHTML == "综合") {  //innerHTML other than innerText since innerText not standard
			$.each(inputsFactor, function() {
					$(this).parent().show()
					$('#applyButton').show()
				}
			);
		} else {
			$.each(inputsFactor, function() {
					$(this).parent().hide()
					$('#applyButton').hide()
				}
			);
		}
	})
	
	$.ajax({ //采用jQuery Ajax方法
		type : "post",
		dataType : "json", //返回数据类型是JSON数据格式
		data : {
			id : id
		}, //传递给Action的参数是param
		url : "findBasicInfo.action", //处理的Action名称
		success : function(data) { //成功的处理函数
			var text = data["rawText"];
			text = text.replace(/。/g, "<br/>")
			document.getElementById("rawText").innerHTML = text;
			
			moment.locale('zh-CN');
			var updateTime = moment(data["timestamp"]);
			$("#updateTime").html(updateTime.fromNow());
		}
	});
	
	<%
	Profile pf = (Profile)request.getAttribute("profile");
	%>
    var locationList = function() {
    	var object = {};
    	var provinceList = [];
    	var cityList = [];
    	<%
    	if (pf.getOfficeRecord() != null)
    	for (int i = 0; i < pf.getOfficeRecord().size(); i++) {					
    		String province = pf.getOfficeRecord().get(i).getProvince();
    		String city = pf.getOfficeRecord().get(i).getCity();
    		String cityOriginal = city;
    		if (!city.equals("-") && !city.endsWith("市")) {
    			city += "市";
    		}
    	%>
    		if ('<%=province%>' !== '-')
    			provinceList.push({name:'<%=province%>',selected:true});
    		if ('<%=cityOriginal%>' == '北京' || '<%=city%>' == '天津' || '<%=city%>' == '重庆' || '<%=city%>' == '上海')
    			provinceList.push({name:'<%=cityOriginal%>',selected:true});
    		if ('<%=city%>' !== '-')
    			cityList.push({name:'<%=city%>',selected:true});
    	<%
    	}
    	%>
    	object.province = provinceList;
    	object.city = cityList;
    	return object;
    }();
    
    function findLocationByRank(rank) {
    	var list = [];
    	<%
    	if (pf.getOfficeRecord() != null)
    	for (int i = 0; i < pf.getOfficeRecord().size(); i++) {
    		String rank = pf.getOfficeRecord().get(i).getRank();
    		%>
    		if (rank == '<%=rank%>') {
    		<%

    			String province = pf.getOfficeRecord().get(i).getProvince();
    			String city = pf.getOfficeRecord().get(i).getCity();
    		%>
    			if ('<%=city%>' == '北京' || '<%=city%>' == '天津' || '<%=city%>' == '重庆' || '<%=city%>' == '上海')
    				list.push({name:'<%=city%>',selected:true});
    			list.push({name:'<%=province%>',selected:true});
    		}
    	<%
    	}
    	%>
    	return list;
    };
    
    var contentList = []; // 用于在tooltip中显示具体的工作信息
    var timeList = []; // 用于在tooltip中显示这段工作经历的起始时间
    var startYear = "";
      
	//人物职务升迁图
	var officeChart = echarts.init(document.getElementById('office'));
	var officeChartOption = ({
		title : {
			text : '职位路线图',
			subtext : '1：副科  2：正科  3：副处  4：正处  5：副局  6：正局  7：副部  8：正部  9：副国  10：正国',
			sublink : '',
			x : 'center'
		},
		tooltip : {
			trigger : 'axis',
			axisPointer : { // 坐标轴指示器，坐标轴触发有效
				type : 'shadow' // 默认为直线，可选为：'line' | 'shadow'
			},
			formatter: function (params,ticket,callback) {
	            var year = params[0][1].substring(0, 4);
	            var index = parseInt(year) - parseInt(startYear);
	            var content = contentList[index];
	            content = content.replace(/@/g, "<br/>")
	            return content + timeList[index] + '<br/>级别：' + params[0][2];
	        }
		},
		xAxis : [ {
			type : 'category',
			//splitLine: {show:false},
			data : []
		} ],
		yAxis : [ {
			type : 'value',
			splitNumber : 10,
			axisLabel : {
				interval : 0,
			}
		} ],
		series : [ {
			name : '职级',
			type : 'line',
			symbolSize : 2,
			showAllSymbol : true,
			itemStyle : {
				normal : {
					label : {
						show : false,
						position : 'top'
					},
					color : 'green'
				},
				emphasis: {
			        label: {
			            show: false,
			            position: 'top'
			        }
			    }
			},
			data : []
		} ]
	});
	
	// 职位升迁图的数据计算和设定
	$.ajax({ //采用jQuery Ajax方法
		type : "post",
		dataType : "json", //返回数据类型是JSON数据格式
		data : {
			id : id
		},
		url : "findRankList.action", //处理的Action名称
		success : function(data) { //成功的处理函数
			console.log(data);
			startYear = data["startYear"];
			var endYear = data["endYear"];
						
			var years = [];
			//该人物没有工作记录信息
			if (startYear == 'NODATA') {
				years = ['经查询无工作经历数据'];
				officeChartOption.xAxis[0].data = years;
				officeChart.setOption(officeChartOption, true);
				return;
			}
			
			for (var i = startYear; i <= endYear; i++) {
				years.push(i + '年');
			}
			
			var rankList = data["rankList"];
			var color = 'green';
			//判断rankList的升降趋势，确定显示折线的颜色
			for (var i = 0; i < rankList.length - 1; i++) {
				if (rankList[i] > rankList[i + 1]) {
					color = 'red';
					break;
				}
			}
			officeChartOption.series[0].itemStyle.normal.color = color;
			
			//消除data["rankList"]中的级别0
			for (var i in rankList)
				if (rankList[i] == "0")
					rankList[i] = "-";
			
			officeChartOption.xAxis[0].data = years;
			officeChartOption.series[0].data = rankList;
			officeChart.setOption(officeChartOption, true);
			
			contentList = data["contentList"];
			timeList = data["timeList"];
		}
	});
	
	//升迁图动态联动，响应点击事件
	function officeFocus(param) {
		var year = param.name;
		var rank = param.value;
		
		//根据点击处的级别和年份查找工作地点并在地图中高亮显示
		var list = findLocationByRank(rank);
		var mapOption = mapChart.getOption();
		mapChart.clear();

		mapOption.series[0].data = list;
		mapChart.setOption(mapOption, true);
	};
	officeChart.on(echarts.config.EVENT.CLICK, officeFocus);
	
	//人物地图地点
	//locationList.province转换为markPoint使用的数据信息
	var markPointList = [];
	for (var i in locationList.province) {
		//{name: "广东", value: "广东"},
		var loc = {};
		loc.name = locationList.province[i].name;
		loc.value = locationList.province[i].name;
		markPointList.push(loc);
	}
	var markLineList = [];
	for (var i = 0; i < locationList.province.length; i++) {
		if (i + 1 < locationList.province.length) {
			if (locationList.province[i].name == locationList.province[i + 1].name)
				continue;
			
			var list = [];
			var loc1 = {};
			loc1.name = locationList.province[i].name;
			var loc2 = {};
			loc2.name = locationList.province[i+1].name;
			
			list.push(loc1);
			list.push(loc2);
			markLineList.push(list);
		}
	}

	var provinceNameMap = {
		"北京": "京",
		"天津": "津",
		"上海": "沪",
		"重庆": "渝",
		"内蒙古": "蒙",
		"新疆": "新",
		"西藏": "藏",
		"宁夏": "宁",
		"广西": "桂",
		"香港": "港",
		"澳门": "澳",
		"黑龙江": "黑",
		"吉林": "吉",
		"辽宁": "辽",
		"山西": "晋",
		"河北": "冀",
		"青海": "青",
		"山东": "鲁",
		"河南": "豫",
		"江苏": "苏",
		"安徽": "皖",
		"浙江": "浙",
		"福建": "闽",
		"江西": "赣",
		"湖南": "湘",
		"湖北": "鄂",
		"广东": "粤",
		"海南": "琼",
		"甘肃": "甘",
		"陕西": "陕",
		"贵州": "黔",
		"云南": "滇",
		"四川": "川",
		"台湾": "台",
		"南海诸岛": ""
	};
	
	var mapChart = echarts.init(document.getElementById('map'));
	var mapOption = {
	    title : {
	        text: '工作地分布图',
	        x:'center'
	    },
		tooltip : {
			trigger : 'item',
			formatter : '{b}<br/>点击进入该省地图'
		},
		series : [ {
			name : '中国',
			type : 'map',
			mapType : 'china',
			selectedMode : 'multiple',
			itemStyle : {
				normal : {
					label : {
						show : true,
						formatter:  function(params) {
							return provinceNameMap[params];
						}
					}
				},
				emphasis : {
					label : {
						show : true
					}
				}
			},
			data : [],
			geoCoord : {
				"海门" : [ 121.15, 31.89 ],
				"鄂尔多斯" : [ 109.781327, 39.608266 ],
				"招远" : [ 120.38, 37.35 ],
				"舟山" : [ 122.207216, 29.985295 ],
				"齐齐哈尔" : [ 123.97, 47.33 ],
				"盐城" : [ 120.13, 33.38 ],
				"赤峰" : [ 118.87, 42.28 ],
				"青岛" : [ 120.33, 36.07 ],
				"乳山" : [ 121.52, 36.89 ],
				"金昌" : [ 102.188043, 38.520089 ],
				"泉州" : [ 118.58, 24.93 ],
				"莱西" : [ 120.53, 36.86 ],
				"日照" : [ 119.46, 35.42 ],
				"胶南" : [ 119.97, 35.88 ],
				"南通" : [ 121.05, 32.08 ],
				"拉萨" : [ 91.11, 29.97 ],
				"西藏" : [ 91.11, 29.97 ],
				"云浮" : [ 112.02, 22.93 ],
				"梅州" : [ 116.1, 24.55 ],
				"文登" : [ 122.05, 37.2 ],
				"上海" : [ 121.48, 31.22 ],
				"攀枝花" : [ 101.718637, 26.582347 ],
				"威海" : [ 122.1, 37.5 ],
				"承德" : [ 117.93, 40.97 ],
				"厦门" : [ 118.1, 24.46 ],
				"汕尾" : [ 115.375279, 22.786211 ],
				"潮州" : [ 116.63, 23.68 ],
				"丹东" : [ 124.37, 40.13 ],
				"太仓" : [ 121.1, 31.45 ],
				"曲靖" : [ 103.79, 25.51 ],
				"烟台" : [ 121.39, 37.52 ],
				"福州" : [ 119.3, 26.08 ],
				"福建" : [ 119.3, 26.08 ],
				"瓦房店" : [ 121.979603, 39.627114 ],
				"即墨" : [ 120.45, 36.38 ],
				"抚顺" : [ 123.97, 41.97 ],
				"玉溪" : [ 102.52, 24.35 ],
				"张家口" : [ 114.87, 40.82 ],
				"阳泉" : [ 113.57, 37.85 ],
				"莱州" : [ 119.942327, 37.177017 ],
				"湖州" : [ 120.1, 30.86 ],
				"汕头" : [ 116.69, 23.39 ],
				"昆山" : [ 120.95, 31.39 ],
				"宁波" : [ 121.56, 29.86 ],
				"湛江" : [ 110.359377, 21.270708 ],
				"揭阳" : [ 116.35, 23.55 ],
				"荣成" : [ 122.41, 37.16 ],
				"连云港" : [ 119.16, 34.59 ],
				"葫芦岛" : [ 120.836932, 40.711052 ],
				"常熟" : [ 120.74, 31.64 ],
				"东莞" : [ 113.75, 23.04 ],
				"河源" : [ 114.68, 23.73 ],
				"淮安" : [ 119.15, 33.5 ],
				"泰州" : [ 119.9, 32.49 ],
				"南宁" : [ 108.33, 22.84 ],
				"广西" : [ 108.33, 22.84 ],
				"营口" : [ 122.18, 40.65 ],
				"惠州" : [ 114.4, 23.09 ],
				"江阴" : [ 120.26, 31.91 ],
				"蓬莱" : [ 120.75, 37.8 ],
				"韶关" : [ 113.62, 24.84 ],
				"嘉峪关" : [ 98.289152, 39.77313 ],
				"广州" : [ 113.23, 23.16 ],
				"广东" : [ 113.23, 23.16 ],
				"延安" : [ 109.47, 36.6 ],
				"太原" : [ 112.53, 37.87 ],
				"山西" : [ 112.53, 37.87 ],
				"清远" : [ 113.01, 23.7 ],
				"中山" : [ 113.38, 22.52 ],
				"昆明" : [ 102.73, 25.04 ],
				"云南" : [ 102.73, 25.04 ],
				"寿光" : [ 118.73, 36.86 ],
				"盘锦" : [ 122.070714, 41.119997 ],
				"长治" : [ 113.08, 36.18 ],
				"深圳" : [ 114.07, 22.62 ],
				"珠海" : [ 113.52, 22.3 ],
				"宿迁" : [ 118.3, 33.96 ],
				"咸阳" : [ 108.72, 34.36 ],
				"铜川" : [ 109.11, 35.09 ],
				"平度" : [ 119.97, 36.77 ],
				"佛山" : [ 113.11, 23.05 ],
				"海口" : [ 110.35, 20.02 ],
				"海南" : [ 110.35, 20.02 ],
				"江门" : [ 113.06, 22.61 ],
				"章丘" : [ 117.53, 36.72 ],
				"肇庆" : [ 112.44, 23.05 ],
				"大连" : [ 121.62, 38.92 ],
				"临汾" : [ 111.5, 36.08 ],
				"吴江" : [ 120.63, 31.16 ],
				"石嘴山" : [ 106.39, 39.04 ],
				"沈阳" : [ 123.38, 41.8 ],
				"辽宁" : [ 123.38, 41.8 ],
				"苏州" : [ 120.62, 31.32 ],
				"茂名" : [ 110.88, 21.68 ],
				"嘉兴" : [ 120.76, 30.77 ],
				"长春" : [ 125.35, 43.88 ],
				"吉林" : [ 125.35, 43.88 ],
				"胶州" : [ 120.03336, 36.264622 ],
				"银川" : [ 106.27, 38.47 ],
				"宁夏" : [ 106.27, 38.47 ],
				"张家港" : [ 120.555821, 31.875428 ],
				"三门峡" : [ 111.19, 34.76 ],
				"锦州" : [ 121.15, 41.13 ],
				"南昌" : [ 115.89, 28.68 ],
				"江西" : [ 115.89, 28.68 ],
				"柳州" : [ 109.4, 24.33 ],
				"三亚" : [ 109.511909, 18.252847 ],
				"自贡" : [ 104.778442, 29.33903 ],
				"吉林" : [ 126.57, 43.87 ],
				"阳江" : [ 111.95, 21.85 ],
				"泸州" : [ 105.39, 28.91 ],
				"西宁" : [ 101.74, 36.56 ],
				"青海" : [ 101.74, 36.56 ],
				"宜宾" : [ 104.56, 29.77 ],
				"呼和浩特" : [ 111.65, 40.82 ],
				"内蒙古" : [ 111.65, 40.82 ],
				"成都" : [ 104.06, 30.67 ],
				"四川" : [ 104.06, 30.67 ],
				"大同" : [ 113.3, 40.12 ],
				"镇江" : [ 119.44, 32.2 ],
				"桂林" : [ 110.28, 25.29 ],
				"张家界" : [ 110.479191, 29.117096 ],
				"宜兴" : [ 119.82, 31.36 ],
				"北海" : [ 109.12, 21.49 ],
				"西安" : [ 108.95, 34.27 ],
				"陕西" : [ 108.95, 34.27 ],
				"金坛" : [ 119.56, 31.74 ],
				"东营" : [ 118.49, 37.46 ],
				"牡丹江" : [ 129.58, 44.6 ],
				"遵义" : [ 106.9, 27.7 ],
				"绍兴" : [ 120.58, 30.01 ],
				"扬州" : [ 119.42, 32.39 ],
				"常州" : [ 119.95, 31.79 ],
				"潍坊" : [ 119.1, 36.62 ],
				"重庆" : [ 106.54, 29.59 ],
				"台州" : [ 121.420757, 28.656386 ],
				"南京" : [ 118.78, 32.04 ],
				"江苏" : [ 118.78, 32.04 ],
				"滨州" : [ 118.03, 37.36 ],
				"贵阳" : [ 106.71, 26.57 ],
				"贵州" : [ 106.71, 26.57 ],
				"无锡" : [ 120.29, 31.59 ],
				"本溪" : [ 123.73, 41.3 ],
				"克拉玛依" : [ 84.77, 45.59 ],
				"渭南" : [ 109.5, 34.52 ],
				"马鞍山" : [ 118.48, 31.56 ],
				"宝鸡" : [ 107.15, 34.38 ],
				"焦作" : [ 113.21, 35.24 ],
				"句容" : [ 119.16, 31.95 ],
				"北京" : [ 116.46, 39.92 ],
				"徐州" : [ 117.2, 34.26 ],
				"衡水" : [ 115.72, 37.72 ],
				"包头" : [ 110, 40.58 ],
				"绵阳" : [ 104.73, 31.48 ],
				"乌鲁木齐" : [ 87.68, 43.77 ],
				"新疆" : [ 87.68, 43.77 ],
				"枣庄" : [ 117.57, 34.86 ],
				"杭州" : [ 120.19, 30.26 ],
				"浙江" : [ 120.19, 30.26 ],
				"淄博" : [ 118.05, 36.78 ],
				"鞍山" : [ 122.85, 41.12 ],
				"溧阳" : [ 119.48, 31.43 ],
				"库尔勒" : [ 86.06, 41.68 ],
				"安阳" : [ 114.35, 36.1 ],
				"开封" : [ 114.35, 34.79 ],
				"济南" : [ 117, 36.65 ],
				"山东" : [ 117, 36.65 ],
				"德阳" : [ 104.37, 31.13 ],
				"温州" : [ 120.65, 28.01 ],
				"九江" : [ 115.97, 29.71 ],
				"邯郸" : [ 114.47, 36.6 ],
				"临安" : [ 119.72, 30.23 ],
				"兰州" : [ 103.73, 36.03 ],
				"甘肃" : [ 103.73, 36.03 ],
				"沧州" : [ 116.83, 38.33 ],
				"临沂" : [ 118.35, 35.05 ],
				"南充" : [ 106.110698, 30.837793 ],
				"天津" : [ 117.2, 39.13 ],
				"富阳" : [ 119.95, 30.07 ],
				"泰安" : [ 117.13, 36.18 ],
				"诸暨" : [ 120.23, 29.71 ],
				"郑州" : [ 113.65, 34.76 ],
				"河南" : [ 113.65, 34.76 ],
				"哈尔滨" : [ 126.63, 45.75 ],
				"黑龙江" : [ 126.63, 45.75 ],
				"聊城" : [ 115.97, 36.45 ],
				"芜湖" : [ 118.38, 31.33 ],
				"唐山" : [ 118.02, 39.63 ],
				"平顶山" : [ 113.29, 33.75 ],
				"邢台" : [ 114.48, 37.05 ],
				"德州" : [ 116.29, 37.45 ],
				"济宁" : [ 116.59, 35.38 ],
				"荆州" : [ 112.239741, 30.335165 ],
				"宜昌" : [ 111.3, 30.7 ],
				"义乌" : [ 120.06, 29.32 ],
				"丽水" : [ 119.92, 28.45 ],
				"洛阳" : [ 112.44, 34.7 ],
				"秦皇岛" : [ 119.57, 39.95 ],
				"株洲" : [ 113.16, 27.83 ],
				"石家庄" : [ 114.48, 38.03 ],
				"河北" : [ 114.48, 38.03 ],
				"莱芜" : [ 117.67, 36.19 ],
				"常德" : [ 111.69, 29.05 ],
				"保定" : [ 115.48, 38.85 ],
				"湘潭" : [ 112.91, 27.87 ],
				"金华" : [ 119.64, 29.12 ],
				"岳阳" : [ 113.09, 29.37 ],
				"长沙" : [ 113, 28.21 ],
				"湖南" : [ 113, 28.21 ],
				"衢州" : [ 118.88, 28.97 ],
				"廊坊" : [ 116.7, 39.53 ],
				"菏泽" : [ 115.480656, 35.23375 ],
				"合肥" : [ 117.27, 31.86 ],
				"安徽" : [ 117.27, 31.86 ],
				"武汉" : [ 114.31, 30.52 ],
				"湖北" : [ 114.31, 30.52 ],
				"大庆" : [ 125.03, 46.58 ]
			},
			markPoint : {
                symbolSize: 10,       // 标注大小，半宽（半径）参数，当图形为方向或菱形则总宽度为symbolSize * 2
                itemStyle: {
                    normal: {
                        borderColor: '#87cefa',
                        borderWidth: 1,            // 标注边线线宽，单位px，默认为1
                        color:'skyblue',
                        label: {
                        	show: true,
                            formatter: function(params, ticket, callback) {
                            	console.log(params);
                            	return params.value;
                            }
                        }
                    },
                    emphasis: {
                        borderColor: '#1e90ff',
                        borderWidth: 5,
                    }
                },
                data : markPointList
            },
            markLine : {
            	smooth:true,
                effect : {
                    show: true,
                    scaleSize: 1,
                    period: 30,
                    color: '#fff',
                    shadowBlur: 10
                },
                itemStyle : {
                    normal: {
                        borderWidth:1,
                        lineStyle: {
                            type: 'solid',
                            shadowBlur: 10
                        },
                        color:'#6495ed'
                    }
                },
                data : markLineList
            }
		}]
	};
	mapChart.setOption(mapOption, true);
	//修改点击事件为省内地图切换
	//地图点击，显示在该省出生的所有人
	/*
	mapChart.on(echarts.config.EVENT.MAP_SELECTED, function (param){
	    var selected = param.target;
	    window.location.href = "./provinceSearch.action?province=" + encodeURIComponent(selected);
	});
	*/
	var ecConfig = echarts.config;
	var zrEvent = zrender.tool.event;
	var curIndx = 0;
	var mapType = [
	    'china',
	    // 23个省
	    '广东', '青海', '四川', '海南', '陕西', 
	    '甘肃', '云南', '湖南', '湖北', '黑龙江',
	    '贵州', '山东', '江西', '河南', '河北',
	    '山西', '安徽', '福建', '浙江', '江苏', 
	    '吉林', '辽宁', '台湾',
	    // 5个自治区
	    '新疆', '广西', '宁夏', '内蒙古', '西藏', 
	    // 4个直辖市
	    '北京', '天津', '上海', '重庆',
	    // 2个特别行政区
	    '香港', '澳门'
	];
	mapChart.on(ecConfig.EVENT.MAP_SELECTED, function (param){
	    var len = mapType.length;
	    var mt = mapType[curIndx % len];
	    if (mt == 'china') {
	        // 全国选择时指定到选中的省份
	        var selected = param.selected;
	        for (var i in selected) {
	            if (selected[i]) {
	                mt = i;
	                while (len--) {
	                    if (mapType[len] == mt) {
	                        curIndx = len;
	                    }
	                }
	                break;
	            }
	        }
	        mapOption.tooltip.formatter = '{b}<br/>点击返回全国';
	        mapOption.series[0].data = locationList.city;
	        mapOption.series[0].markPoint.data = [];
	    }
	    else {
	        curIndx = 0;
	        mt = 'china';
	        mapOption.tooltip.formatter = '{b}<br/>点击进入该省';
	        mapOption.series[0].data = [];
	        mapOption.series[0].markPoint.data = markPointList;
	    }
	    mapOption.series[0].mapType = mt;
	    mapChart.setOption(mapOption, true);
	});
	
	// 各类小关系图的通用选项区，提供一个基础模板
	var generalOption = {
		title : {
			text : '人物关系',
			subtext : '',
			x : 'right',
			y : 'bottom'
		},
		tooltip : {
			trigger : 'item',
			formatter : '{a} : {b}'
		},
		legend : {
			x : 'left',
			data : [ '同乡', '' ]
		},
		backgroundColor: '#F0F0F0',
		series : [ {
			type : 'force',
			name : "人物关系",
			categories : [ {
				name : '人物'
			}, {
				name : '同乡'
			}, {
				name : ''
			} ],
			itemStyle : {
				normal : {
					label : {
						show : true,
						textStyle : {
							color : '#333'
						}
					},
					nodeStyle : {
						brushType : 'both',
						strokeColor : 'rgba(255,215,0,0.4)',
						lineWidth : 1
					}
				},
				emphasis : {
					label : {
						show : false
					// textStyle: null      // 默认使用全局文本样式，详见TEXTSTYLE
					},
					nodeStyle : {
					//r: 30
					},
					linkStyle : {}
				}
			},
			useWorker : true,
			symbolSize : 15,
			gravity : 1.1,
			scaling : 1.2,
			nodes : [],
			links : []
		} ]
	};
	
	
	// 所有人物关系图的动态联动，响应点击事件
	function relationFocus(param) {
		var data = param.data;
		window.location.href = "./searchByName.action?name=" + encodeURIComponent(data.name);
	};
	
	var locationRelationChart = echarts.init(document.getElementById('locationRelation'));
	locationRelationChart.on(echarts.config.EVENT.CLICK, relationFocus);
	
	// 同乡关系图的数据计算和设定
	$.ajax({ //采用jQuery Ajax方法
		type : "post",
		dataType : "json", //返回数据类型是JSON数据格式
		data : {
			id : id
		},
		url : "findLocMate.action", //处理的Action名称
		success : function(data) { //成功的处理函数
			//console.log(data);
			var relationOption = generalOption;
			relationOption.series[0].links = data;
			//处理data为符合nodes的格式
			var nodes = [];
			var i;
			for (i in data) {
				//{category:3, name: '申维辰', value : 10}
				var obj = {};
				obj.category = 1;
				obj.name = data[i].source;
				obj.value = 10;
				nodes.push(obj);
			}
			var obj = {};
			obj.category = 0;
			obj.name = name;
			obj.value = 20;
			nodes.push(obj)
			relationOption.series[0].nodes = nodes;
			
			relationOption.legend.data[0] = '同乡';
			relationOption.legend.data[1] = '';
			relationOption.series[0].categories[1].name = '同乡';
			relationOption.series[0].categories[2].name = '';
			
			locationRelationChart.setOption(relationOption, true);
		}
	});
	
	// 按时间关系计算的同事关系图
	var colleagueRelationChart = echarts.init(document.getElementById('colleagueRelation'));
	$.ajax({ //采用jQuery Ajax方法
		type : "post",
		dataType : "json", //返回数据类型是JSON数据格式
		data : {
			id : id
		},
		url : "findColleague.action", //处理的Action名称
		success : function(data) { //成功的处理函数
			var colleagueOption = generalOption;
			//处理data为符合nodes的格式
			var nodes = [];
			//加入self的node
			var obj = {};
			obj.category = 0;
			obj.name = name;
			obj.value = 20;
			nodes.push(obj);
			
			var i;
			for (i in data) {
				//{category:3, name: '申维辰', value : 10}
				var obj = {};
				obj.category = 1; //直接同事
				data[i].weight = parseInt(data[i].weight);
				if (data[i].weight == 0) {
					obj.category = 2; //间接同事
					data[i].weight = 7;
				}
				
				obj.name = data[i].source;
				obj.value = 10;
				nodes.push(obj);
			}
			
			colleagueOption.series[0].links = data;
			colleagueOption.series[0].nodes = nodes;
			
			colleagueOption.legend.data[0] = '直接同事';
			colleagueOption.legend.data[1] = '间接同事';
			colleagueOption.series[0].categories[1] = {name : '直接同事'};
			colleagueOption.series[0].categories[2] = {name : '间接同事'};
			
			colleagueRelationChart.setOption(colleagueOption, true)
		}
	});
	colleagueRelationChart.on(echarts.config.EVENT.CLICK, relationFocus);
	
	// 按级别关系计算的同事关系图
	var colleagueRelationChart2 = echarts.init(document.getElementById('colleagueRelation2'))
	$.ajax({ //采用jQuery Ajax方法
		type : "post",
		dataType : "json", //返回数据类型是JSON数据格式
		data : {
			id : id
		}, //传递给Action的参数是param
		url : "findRankColleague.action", //处理的Action名称
		success : function(data) { //成功的处理函数
			//console.log(data);
			var colleagueOption2 = generalOption;
			//处理data为符合nodes的格式
			var nodes = [];
			//加入self的node
			var obj = {};
			obj.category = 0;
			obj.name = name;
			obj.value = 20;
			nodes.push(obj);
			
			var i;
			for (i in data) {
				//{category:3, name: '申维辰', value : 10}
				var obj = {};
				obj.category = 1; //直接同事
				obj.name = data[i].source;
				obj.value = 10;
				nodes.push(obj);
			}
			
			colleagueOption2.series[0].links = data;
			colleagueOption2.series[0].nodes = nodes;
			
			colleagueOption2.legend.data[0] = '同事';
			colleagueOption2.legend.data[1] = '';
			colleagueOption2.series[0].categories[1] = {name : '同事'};
			colleagueOption2.series[0].categories[2] = {name : ''};
			
			colleagueRelationChart2.setOption(colleagueOption2, true)
		}
	});
	colleagueRelationChart2.on(echarts.config.EVENT.CLICK, relationFocus);
	
	// 当前学习经历同学关系图
	var schoolMateRelationChart = echarts.init(document.getElementById('currentClassMateRelation'))
	$.ajax({ //采用jQuery Ajax方法
		type : "post",
		dataType : "json", //返回数据类型是JSON数据格式
		data : {
			id : id
		},
		url : "findCurrentSchoolMate.action", //处理的Action名称
		success : function(data) { //成功的处理函数
			var schoolMateOption = generalOption;
			//处理data为符合nodes的格式
			var nodes = [];
			//加入self的node
			var obj = {};
			obj.category = 0; //自己
			obj.name = name;
			obj.value = 20;
			nodes.push(obj);
			
			var i;
			for (i in data) {
				var obj = {};
				obj.category = 1; //直接同学
				data[i].weight = parseInt(data[i].weight);
				
				obj.name = data[i].source;
				obj.value = 10;
				nodes.push(obj);
			}
			
			schoolMateOption.series[0].links = data;
			schoolMateOption.series[0].nodes = nodes;
			
			schoolMateOption.series[0].scaling = 2.0;
			
			schoolMateOption.legend.data[0] = '同学';
			schoolMateOption.legend.data[1] = '';
			schoolMateOption.series[0].categories[1] = {name : '同学'};
			schoolMateOption.series[0].categories[2] = {name : ''};
			
			schoolMateRelationChart.setOption(schoolMateOption, true)
		}
	});
	schoolMateRelationChart.on(echarts.config.EVENT.CLICK, relationFocus);
	
	// 过去学习经历同学关系图
	var schoolMateRelationChart2 = echarts.init(document.getElementById('previousClassMateRelation'))
	$.ajax({ //采用jQuery Ajax方法
		type : "post",
		dataType : "json", //返回数据类型是JSON数据格式
		data : {
			id : id
		},
		url : "findPreviousSchoolMate.action", //处理的Action名称
		success : function(data) { //成功的处理函数
			var schoolMateOption2 = generalOption;
			//处理data为符合nodes的格式
			var nodes = [];
			//加入self的node
			var obj = {};
			obj.category = 0;
			obj.name = name;
			obj.value = 20;
			nodes.push(obj);
			
			var i;
			for (i in data) {
				var obj = {};
				obj.category = 1; // 直接同学，学习时间有交叉
				data[i].weight = parseInt(data[i].weight);
				if (data[i].weight == 0) {
					obj.category = 2; // 间接同学，学习时间没有交叉
					data[i].weight = 10;
				}
				
				obj.name = data[i].source;
				obj.value = 10;
				nodes.push(obj);
			}
			
			schoolMateOption2.series[0].links = data;
			schoolMateOption2.series[0].nodes = nodes;
			
			schoolMateOption2.series[0].scaling = 2.0;
			
			schoolMateOption2.legend.data[0] = '直接同学';
			schoolMateOption2.legend.data[1] = '间接同学';
			schoolMateOption2.series[0].categories[1] = {name : '直接同学'};
			schoolMateOption2.series[0].categories[2] = {name : '间接同学'};
			
			schoolMateRelationChart2.setOption(schoolMateOption2, true)
		}
	});
	schoolMateRelationChart2.on(echarts.config.EVENT.CLICK, relationFocus);
	
	// 综合关系图的选项模板
	var allOption = {
		title : {
			text : '人物关系',
			subtext : '',
			x : 'right',
			y : 'bottom'
		},
		tooltip : {
			trigger : 'item',
			formatter : '{a} : {b}'
		},
		legend : {
			x : 'left',
			data : []
		},
		backgroundColor: '#F0F0F0',
		series : [ {
			type : 'force',
			name : "人物关系",
			categories : [ {
				name : '人物'
			}, {
				name : '同乡'
			}, {
				name : '同事'
			}, {
				name : '同学'
			}, {
				name : '同乡&同事',
				symbol: 'diamond'
			}, {
				name : '同乡&同学',
				symbol: 'diamond'
			}, {
				name : '同事&同学',
				symbol: 'diamond'
			}, {
				name : '同乡&同事&同学',
				symbol: 'star'
			} ],
			itemStyle : {
				normal : {
					label : {
						show : true,
						textStyle : {
							color : '#333',
							fontSize : 8
						}
					},
					nodeStyle : {
						brushType : 'both',
						strokeColor : 'rgba(255,215,0,0.4)',
						lineWidth : 1
					}
				},
				emphasis : {
					label : {
						show : false
					},
					nodeStyle : {
					//r: 30
					},
					linkStyle : {}
				}
			},
			useWorker : true,
			symbolSize : 15,
			gravity : 1.1,
			scaling : 1.2,
			nodes : [],
			links : []
		} ]
	};
	
	//all relation chart
	var allRelationChart = echarts.init(document.getElementById('allRelation'));
	function doApply() {
		$.ajax({ //采用jQuery Ajax方法
			type : "post",
			dataType : "json", //返回数据类型是JSON数据格式
			data : {
				id : id
			},
			url : "findAllRelationship.action", //处理的Action名称
			success : function(data) { //成功的处理函数
				//factorArray中各因子的顺序是
				var factorArray = []
				$.each(inputsFactor, function() {
						factorArray.push($(this).val());
					}
				);
				
				//处理data为符合nodes的格式
				var nodes = [];
				//加入self的node
				var obj = {};
				obj.category = 0;
				obj.name = name;
				obj.value = 20;
				nodes.push(obj);
				
				var dataArray = [];
				for (var i in data) {
					var obj = {};					
					obj.name = i;
					obj.value = 20;		
					
					var weightArray = data[i];
					var flag = [0, 0, 0];
					for (var j in weightArray) {
						weightArray[j] = parseInt(weightArray[j]);
						if (isNaN(weightArray[j]))
							weightArray[j] = 0;
						
						if (weightArray[j] != 0)
							flag[j] = 1;
					}
					
					var arrayItem = {};
					arrayItem.source = i;
					arrayItem.target = name;
					
					arrayItem.weight = weightArray[0] * 12 * factorArray[0] + weightArray[1] * factorArray[1] + weightArray[2] * factorArray[2];
					//arrayItem.weight = '10';
					var category = flag[0] * 4 + flag[1] * 2 + flag[2];
					switch(category) {
					case 0:
						obj.category = 0;
						break;
					case 1: 
						obj.category = 3;
						break;
					case 2:
						obj.category = 2;
						break;
					case 3:
						obj.category = 6;
						break;
					case 4:
						obj.category = 1;
						break;
					case 5:
						obj.category = 5;
						break;
					case 6:
						obj.category = 4;
						break;
					case 7:
						obj.category = 7;
						break;
					}
					if (obj.category != 0) {
						nodes.push(obj);
						dataArray.push(arrayItem);
					}
				}
				
				allOption.series[0].links = dataArray;
				allOption.series[0].nodes = nodes;
				
				allRelationChart.setOption(allOption, true);
			}
		});
	};
	
	doApply();
	
	$('#applyButton').on('click', function () {
		var $btn = $(this).button('loading');
		$btn.button('reset');
		//判断输入因子的合法性
		$.each(inputsFactor, function(i) {
			if (this.value > 1 || this.value < 0) {
				console.log("输入因子不合法！");
				alert("请输入0~1的小数！");
				$.each(inputsFactor, function(){
					this.value = 0.33;
				})
			}
		})
		
		allRelationChart.clear();
		doApply();
	})
	
	//综合人物关系图动态联动，响应点击事件
	allRelationChart.on(echarts.config.EVENT.CLICK, relationFocus);
	
	//These 3 functions are used for relation-charts zooming.
	function biggerVis() {
		$(".author-vis-matrix").css("margin-left", "-200px");
		
		$("#allRelation").removeClass("relation-chart").addClass("relation-chart-large");
		$("#locationRelation").removeClass("relation-chart").addClass("relation-chart-large");
		$("#colleagueRelation").removeClass("relation-chart").addClass("relation-chart-large");
		$("#colleagueRelation2").removeClass("relation-chart").addClass("relation-chart-large");
		$("#currentClassMateRelation").removeClass("relation-chart").addClass("relation-chart-large");
		$("#previousClassMateRelation").removeClass("relation-chart").addClass("relation-chart-large");
		
		allRelationChart.resize();
		locationRelationChart.resize();
		colleagueRelationChart.resize()
		colleagueRelationChart2.resize();
		schoolMateRelationChart.resize();
		schoolMateRelationChart2.resize();
	}
	function smallerVis() {
		$(".author-vis-matrix").css("margin-left", "0px");
		
		$("#allRelation").removeClass("relation-chart-large").addClass("relation-chart");
		$("#locationRelation").removeClass("relation-chart-large").addClass("relation-chart");
		$("#colleagueRelation").removeClass("relation-chart-large").addClass("relation-chart");
		$("#colleagueRelation2").removeClass("relation-chart-large").addClass("relation-chart");
		$("#currentClassMateRelation").removeClass("relation-chart-large").addClass("relation-chart");
		$("#previousClassMateRelation").removeClass("relation-chart-large").addClass("relation-chart");
		
		allRelationChart.resize();
		locationRelationChart.resize();
		colleagueRelationChart.resize()
		colleagueRelationChart2.resize();
		schoolMateRelationChart.resize();
		schoolMateRelationChart2.resize();
	}
	var visSizeState = "small";
	function toggleVis() {
		if (visSizeState == "small") {		
			biggerVis();
			$("#visToggleSizeIcon").removeClass("glyphicon-resize-full").addClass("glyphicon-resize-small");
			visSizeState = "full";	
		} else {
			smallerVis();
			$("#visToggleSizeIcon").removeClass("glyphicon-resize-small").addClass("glyphicon-resize-full");
			visSizeState = "small";
		}
	}
	
	function deleteProfile() {
		console.log("已确认删除该人信息");
		console.log("已删除" + id + name);
		$('#myModal').modal('hide');
		window.location.href = "./delete.action?id=" + id;
	}
	
	function setLevel(levelArg) {
		//do some action here
		$.ajax({ //采用jQuery Ajax方法
			type : "post",
			dataType : "json", //返回数据类型是JSON数据格式
			data : {
				id : id,
				level : levelArg
			},
			url : "updateLevel.action",
			success : function(data) {
				document.getElementById("currentLevel").innerHTML = levelArg;
				alert("级别已调整");
			}
		});
	}
</script>

</body>
</html>
