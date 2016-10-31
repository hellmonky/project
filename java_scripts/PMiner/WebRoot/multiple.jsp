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

    <title>搜索结果</title>
    <meta name="keywords" content="pminer,search,result,结果">
    <meta name="description" content="Pminer Search Result">
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
    <script src="js/moment-with-locales.min.js"></script>
  </head>
  <body>
    <div class="container-fluid">
      <div class="row" style="color:white;background-color:#4791D2;" id="table">
        <div class="col-md-2 col-md-offset-1" style="padding:30px;">
          <span class="glyphicon glyphicon-home">
            <a href="index.jsp" style="color:white;">回到首页</a>
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

        <div class="col-md-4" style="padding:20px;">
          <h5 id="digitaltime"></h5>
        </div>
      </div>

      <br>

      <div class="row">
        <s:iterator value="#request.profileList" id='profile'>
        <div class="col-md-2">
          <a href="#" class="thumbnail">
            <img src="<s:url action='fileDisplay.action?id=%{#request.profile.id}'/>" class="img-responsive" style="height:200px" alt="Responsive image"/>
          </a>
        </div>
        <div class="col-md-4">
          <div class="panel panel-default">
            <div class="panel-heading">
              <span class="glyphicon glyphicon-user"></span>&nbsp;&nbsp;基本信息
              <button class="btn btn-info btn-sm" style="margin-left:100px" onclick="window.location.href='searchById.action?id=<s:property value="#request.profile.id"/>'">
              <span class="glyphicon glyphicon-edit"></span>&nbsp;&nbsp;点击查看详细信息
              </button>
            </div>

            <table class="table table-hover">
              <tr>
                <td>姓名</td>
                <td id="profileName"><s:property value="#profile.name"/></td>
                <td>性别</td>
                <td><s:property value="#profile.gender"/></td>
              </tr>
              <tr>
                <td>民族</td>
                <td><s:property value="#profile.nation"/></td>
                <td>籍贯</td>
                <td><s:property value="#profile.birthProvince"/>&nbsp<s:property value="#profile.birthCity"/></td>
              </tr>
              <tr>
                <td>年龄</td>
                <td><s:property value="#profile.age"/></td>
                <td>出生日期</td>
                <td><s:property value="#profile.birthDate"/></td>
              </tr>
              <tr>
                <td>入党日期</td>
                <td><s:property value="#profile.partyDate"/></td>
                <td>工作日期</td>
                <td><s:property value="#profile.workDate"/></td>
              </tr>
              <tr>
                <td>现任职务</td>
                <td colspan="3">
                  <s:iterator value="#profile.officeRecord" id='record' status="st">
                  <s:if test="#st.Last">
                  <s:iterator value='#record.tupleList' id="tuple">
                  <s:property value='#tuple.content'/>
                  </s:iterator>
                  </s:if>
                  </s:iterator>
                </td>
              </tr>
              <tr>
                <td>档案级别</td>
                <td id="currentLevel"><s:property value="#request.profile.level"/></td>
                <td>更新日期</td>
                <td>
                  <script>
                    moment.locale('zh-CN');
                    var updateTime = moment(<s:property value="#request.profile.timestamp.getTime()"/>);
                    document.write(updateTime.fromNow());
                  </script>
                </td>
              </tr>
            </table>
          </div>
        </div>
        </s:iterator>
      </div>
    </div>

    <script src="js/jquery.min.js"></script>
    <script src="js/bootstrap.min.js"></script>
    <script src="js/digitaltime.js"></script>
    <script src="js/typeahead.bundle.min.js"></script>
    <script src="js/input-autocomplete.js"></script>
  </body>
</html>