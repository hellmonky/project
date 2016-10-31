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
    <!-- Standard Meta -->
    <meta charset="utf-8">
    <base href="<%=basePath%>">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">

    <title>PMiner | 搜索结果</title>
    <meta name="keywords" content="pminer,search,profile">
    <meta name="description" content="Pminer search page">
    <meta name="content-type" content="text/html; charset=UTF-8">

    <link rel="stylesheet" type="text/css" href="css/semantic.min.css">
    <link rel="stylesheet" type="text/css" href="css/docs.css">
    <script src="js/jquery.min.js"></script>

    <style type='text/css'>
    .full.height {
      position: relative;
      height: auto;
      min-height: 100%
    }
    #footer {
      position: absolute;
      bottom: 0;
      height: 70px;
      width: 100%;
    }
    </style>
  </head>

  <body ontouchstart>
    <div id="menu" class="ui fixed blue large inverted menu">
      <div class="container">
        <a class="header item" href="#">PMiner</a>
        <a class="active item" href="index.jsp">
          <i class="home icon"></i> 首页
        </a>
        <div class="right menu">
          <div class="right item">
            <a class="ui black button" href="logout.action">注销</a>
          </div>
          <div class="item" style="height:41px;padding-top:1px;">
            <form action="searchByKeyword.action" method="post">
              <div class="ui action input">
                <input placeholder="综合搜索" type="text" id="searchName" name="name">
                <button class="ui green button" id="searchButton">搜索</button>
              </div>
            </form>
          </div>
        </div>
      </div>
    </div>

    <div class="full height">
      <div class="main container" style="padding-top: 70px; padding-bottom: 70px">
        <div class="ui two column grid">
          <div class="column">
            <div class="ui fluid action input">
            <input type="text" placeholder="Search Now...">
            <button class="ui teal icon button">
              <i class="search icon"></i>
            </button>
          </div>
          </div>
          <div class="two column row">
            <div class="column">
              <h2>搜索结果</h2>
            </div>
          </div>
        </div>

        <div class="ui divided items">
          <s:iterator value="#request.profileList" id='profile'>
          <div class="item">
            <div class="ui tiny image">
              <img src="<s:url action='fileDisplay.action?id=%{#request.profile.id}'/>">
            </div>
            <div class="content">
              <a class="header" href="searchByIdNew.action?id=<s:property value="#profile.id"/>"><s:property value="#profile.name"/></a>
              <div class="meta">
                <span><s:property value="#profile.gender"/></span>
                <span><s:property value="#profile.birthProvince"/></span>
                <span><s:property value="#profile.birthCity"/></span>
              </div>
              <div class="description">
                <p>
                  <s:iterator value="#request.profile.latestOfficeRecord.tupleList" id='tuple'>
                    <s:property value='#tuple.content'/>
                  </s:iterator>
                </p>
              </div>
              <div class="extra">
                <div class="ui blue labels">
                  <a class="ui label">
                    Fun
                  </a>
                  <a class="ui label">
                    Happy
                    <div class="detail">22</div>
                  </a>
                  <a class="ui label">
                    Smart
                  </a>
                  <a class="ui label">
                    Insane
                  </a>
                  <a class="ui label">
                    Exciting
                  </a>
                </div>
              </div>
            </div>
          </div>
          </s:iterator><!-- end of struts tag iterator -->
        </div><!-- ui divided item -->
      </div>

      <div id="footer" class="ui inverted blue vertical segment" style="padding-top: 2em; padding-bottom:4em">
        <div class="container">
          <div class="ui stackable inverted divided relaxed grid">
            <div class="six wide column">
              <h4 class="ui inverted header">
                2014-2015 PMiner. <i class="copyright icon"></i> ALL Rights Reserved.
              </h4>
            </div>
            <div class="four wide column">
              <h4>关于PMiner</h4>
            </div>
            <div class="four wide column">
              <h4>开发成员</h4>
            </div>
          </div>
        </div><!-- container -->
      </div><!-- footer -->
    </div>

    <script>

    </script>
  </body>
</html>
