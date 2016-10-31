<%@ page language="java" import="java.util.*" contentType="text/html;charset=UTF-8" pageEncoding="UTF-8"%>
<%
String path = request.getContextPath();
String basePath = request.getScheme()+"://"+request.getServerName()+":"+request.getServerPort()+path+"/";
%>
<%@ taglib prefix="s" uri="/struts-tags" %>
<%@ taglib prefix="shiro" uri="http://shiro.apache.org/tags" %>
<!DOCTYPE html>
<html lang="zh-cn">
  <head>
    <!-- Standard Meta -->
    <meta charset="utf-8">
    <base href="<%=basePath%>">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">

    <title>登录</title>
    <meta name="keywords" content="pminer,login,注册,登录">
    <meta name="description" content="Pminer Login">
    <meta name="content-type" content="text/html; charset=UTF-8">

    <!-- Bootstrap -->
    <link href="css/bootstrap.min.css" rel="stylesheet">
    <link href="css/main.css" rel="stylesheet">
    <link href="css/login.css" rel="stylesheet">
    <!-- HTML5 Shim and Respond.js IE8 support of HTML5 elements and media queries -->
    <!-- WARNING: Respond.js doesn't work if you view the page via file:// -->
    <!--[if lt IE 9]>
    <script src="http://cdn.bootcss.com/html5shiv/3.7.2/html5shiv.min.js"></script>
    <script src="http://cdn.bootcss.com/respond.js/1.4.2/respond.min.js"></script>
    <![endif]-->
    <style type='text/css'>
    .navbar-fixed-top .container {
    width: 960px;
    position: relative;
    }
    .container {
    width: 960px;
    position: relative;
    }
    </style>
  </head>
  <body>
    <nav class="navbar navbar-inverse navbar-fixed-top" role="navigation">
      <div class="container">
        <div class="navbar-header">
          <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#navbar" aria-expanded="false" aria-controls="navbar">
          <span class="sr-only">Toggle navigation</span>
          <span class="icon-bar"></span>
          <span class="icon-bar"></span>
          <span class="icon-bar"></span>
          </button>
          <a class="navbar-brand" href="#">PMiner</a>
        </div>
        <div id="navbar" class="collapse navbar-collapse">
          <ul class="nav navbar-nav">
            <li class="active"><a href="#">首页</a></li>
            <li><a href="#contact">更多内容</a></li>
            <li><a href="#about">关于我们</a></li>
          </ul>
        </div><!--/.nav-collapse -->
      </div>
    </nav>

    <div class="container" style="padding-top:50px">
      <div class="row">
        <div class="col-md-5">
          <h1>欢迎访问PMiner</h1>
          <P></p>
          <h3>现在登录以体验更多功能！</h3>
        </div>

        <div class="col-md-7">
          <shiro:user>
          <div class="panel panel-default">
            <div class="panel-heading">欢迎回来！<shiro:principal/></div>
            <div class="panel-body">
              不是<shiro:principal/>？点击<a href="logout.action">这里</a>登录.
            </div>
          </div>
          </shiro:user>

          <shiro:guest>
          <form class="form-signin" action="login.action" method="post">
            <h3 class="form-signin-heading text-center">请先登录</h3>
            <input type="text" class="form-control" placeholder="用户名" required autofocus name="username">
            <input type="password" class="form-control" placeholder="密码" required name="password">
            <div class="checkbox">
              <label>
                <input type="checkbox" value="true" name="rememberMe" checked>记住我？
              </label>
            </div>
            <button class="btn btn-lg btn-primary btn-block" type="submit">登录</button>
          </form>
          </shiro:guest>
        </div>
      </div>
    </div>
    <!-- /container -->

    <script src="js/jquery.min.js"></script>
    <script src="js/bootstrap.min.js"></script>
  </body>
</html>