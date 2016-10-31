<%@ page language="java" import="java.util.*" contentType="text/html;charset=UTF-8" pageEncoding="UTF-8"%>
<%
String path = request.getContextPath();
String basePath = request.getScheme()+"://"+request.getServerName()+":"+request.getServerPort()+path+"/";
%>
<%@ taglib prefix="s" uri="/struts-tags" %>
<%@ page import="iscas.entity.Profile"%>
<%@ page import="iscas.entity.Tuple"%>
<!DOCTYPE html>
<html lang="zh-cn">
  <head>
    <meta charset="utf-8">
    <base href="<%=basePath%>">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">

    <title>省份搜索结果</title>
    <meta name="keywords" content="pminer,search,result,province">
    <meta name="description" content="Pminer Search Result for province">
    <meta name="content-type" content="text/html; charset=UTF-8">

    <!-- Bootstrap -->
    <link href="css/bootstrap.min.css" rel="stylesheet">
    <link href="css/main.css" rel="stylesheet">
    <!-- HTML5 Shim and Respond.js IE8 support of HTML5 elements and media queries -->
    <!-- WARNING: Respond.js doesn't work if you view the page via file:// -->
    <!--[if lt IE 9]>
    <script src="http://cdn.bootcss.com/html5shiv/3.7.2/html5shiv.min.js"></script>
    <script src="http://cdn.bootcss.com/respond.js/1.4.2/respond.min.js"></script>
    <![endif]-->
  </head>
  <body>
    <div class="container-fluid">
      <div class="row" style="color:white;background-color:#4791D2;" id="table">
        <div class="col-md-2 col-md-offset-2" style="padding:30px;">
          <span class="glyphicon glyphicon-home">
            <a href="index.html" style="color:white;">回到首页</a>
          </span>
        </div>

        <div class="col-md-3" style="padding:20px;">
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
        <div class="col-md-3 col-md-offset-2">
          <div class="panel panel-primary">
            <div class="panel-heading" id="provinceHead"></div>
            <ul class="list-group" style="overflow:auto; height:500px">
              <s:iterator value="#request.nameList" id='list'>
              <a href="searchById.action?id=<s:property value='#list._id'/>" class="list-group-item">
                <s:property value='#list.name'/>
              </a>
              </s:iterator>
            </ul>
          </div>
        </div>

        <div class="col-md-5">
          <div class="panel panel-primary">
            <div class="panel-heading">
              任免信息
            </div>

          <ul class="list-group" style="overflow:auto; height:500px" id="appointList"></ul>
        </div>
      </div>
    </div>
  </div>
  <script src="js/jquery.min.js"></script>
  <script src="js/bootstrap.min.js"></script>
  <script src="js/digitaltime.js"></script>
  <script src="js/typeahead.bundle.min.js"></script>
  <script src="js/input-autocomplete.js"></script>
  <script>
    var urlParams;
    (window.onpopstate = function() {
      var match,
        pl = /\+/g, // Regex for replacing addition symbol with a space
        search = /([^&=]+)=?([^&]*)/g,
        decode = function(s) {
          return decodeURIComponent(s.replace(pl, " "));
        },
        query = window.location.search.substring(1);
      urlParams = {};
      while (match = search.exec(query))
        urlParams[decode(match[1])] = decode(match[2]);
    })();
    var province = urlParams["province"];
    var peopleNum = <s:property value = '#request.nameList.size()'/> ; //该省的人员总数
    document.getElementById("provinceHead").innerHTML = province + "-人员列表  (" + peopleNum + "人)";

    function getAppointListByProvince() {
      $.ajax({ //采用jQuery Ajax方法
        type: "post",
        dataType: "json", //返回数据类型是JSON数据格式
        data: {
          province: province + "省"
        }, //传递给Action的参数是param
        url: "getAppointListByProvince.action", //处理的Action名称
        success: function(data) { //成功的处理函数
          $("#appointList").html('');
          var object = [];
          data.sort(function(a, b) {
            if (a.act == "任" && b.act == "免")
              return -1;
            else if (a.act == "免" && b.act == "任")
              return 1;
            else
              return 0;
          });
          for (var i in data) {
            console.log(data[i]);
            $("#appointList").append("<a href=\"" + data[i].href + "\" target=\"_blank\" class=\"list-group-item\">" +
              data[i].act + "&nbsp" + data[i].name + "&nbsp" + data[i].position + "&nbsp" + data[i].date +
              "</a>");
          }

        }
      });
    };
    getAppointListByProvince();
  </script>
</body>
</html>

