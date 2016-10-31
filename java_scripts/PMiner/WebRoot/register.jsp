<%@ page language="java" import="java.util.*" contentType="text/html;charset=UTF-8" pageEncoding="UTF-8"%>
<%
String path = request.getContextPath();
String basePath = request.getScheme()+"://"+request.getServerName()+":"+request.getServerPort()+path+"/";
%>
<%@ taglib prefix="s" uri="/struts-tags" %>
<!DOCTYPE html>
<html lang="zh-cn">
  <head>
    <meta charset="utf-8">
    <base href="<%=basePath%>">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">

    <title>PMiner | 注册</title>
    <meta name="keywords" content="pminer,register,PMiner,注册">
    <meta name="description" content="Pminer register">
    <meta name="content-type" content="text/html; charset=UTF-8">

    <!-- Bootstrap -->
    <link href="css/bootstrap.min.css" rel="stylesheet">
    <link href="css/main.css" rel="stylesheet">
    <link href="css/font-awesome.min.css" rel="stylesheer">
    <!-- Include Bootstrap Datepicker -->
    <link rel="stylesheet" href="//cdnjs.cloudflare.com/ajax/libs/bootstrap-datepicker/1.3.0/css/datepicker.min.css">
    <link rel="stylesheet" href="//cdnjs.cloudflare.com/ajax/libs/bootstrap-datepicker/1.3.0/css/datepicker3.min.css">
    <!-- BootstrapValidator CSS -->
    <link rel="stylesheet" href="//cdnjs.cloudflare.com/ajax/libs/jquery.bootstrapvalidator/0.5.3/css/bootstrapValidator.min.css"/>
    <!-- HTML5 Shim and Respond.js IE8 support of HTML5 elements and media queries -->
    <!-- WARNING: Respond.js doesn't work if you view the page via file:// -->
    <!--[if lt IE 9]>
    <script src="http://cdn.bootcss.com/html5shiv/3.7.2/html5shiv.min.js"></script>
    <script src="http://cdn.bootcss.com/respond.js/1.4.2/respond.min.js"></script>
    <![endif]-->
    <script src="js/jquery.min.js"></script>
    <script src="js/bootstrap.min.js"></script>
    <!-- DatePicker JS -->
    <script src="//cdnjs.cloudflare.com/ajax/libs/bootstrap-datepicker/1.3.0/js/bootstrap-datepicker.min.js"></script>
    <script src="//cdnjs.cloudflare.com/ajax/libs/bootstrap-datepicker/1.3.0/js/locales/bootstrap-datepicker.zh-CN.min.js"></script>
    <!-- BootstrapValidator JS -->
    <script src="//cdnjs.cloudflare.com/ajax/libs/jquery.bootstrapvalidator/0.5.3/js/bootstrapValidator.min.js"></script>
    <script src="//cdnjs.cloudflare.com/ajax/libs/jquery.bootstrapvalidator/0.5.3/js/language/zh_CN.min.js"></script>
    <style type='text/css'>
    .navbar-fixed-top .container {
    width: 960px;
    position: relative;
    }
    .container {
    width: 960px;
    position: relative;
    }
    /**
    * Override feedback icon position
    * See http://bootstrapvalidator.com/examples/adjusting-feedback-icon-position/
    */
    #registrationForm .form-control-feedback {
    top: 0;
    right: -15px;
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
          <h3>现在注册以体验更多功能！</h3>
        </div>
        <div class="col-md-7"><!-- 注册表单 -->
          <h1 class="text-center">注册</h1>
          <form action="register.action" id="registrationForm" method="post" class="form-horizontal">
            <div class="form-group">
              <label class="col-sm-3 control-label">用户名</label>
              <div class="col-sm-7">
                <input type="text" class="form-control" name="username" />
              </div>
            </div>
            <div class="form-group">
              <label class="col-sm-3 control-label">邮箱</label>
              <div class="col-sm-7">
                <input type="text" class="form-control" name="email" />
              </div>
            </div>
            <div class="form-group">
              <label class="col-sm-3 control-label">密码</label>
              <div class="col-sm-7">
                <input type="password" class="form-control" name="password" />
              </div>
            </div>
            <div class="form-group">
              <label class="col-sm-3 control-label">确认密码</label>
              <div class="col-sm-7">
                <input type="password" class="form-control" name="confirmPassword" />
              </div>
            </div>
            <div class="form-group">
              <label class="col-sm-3 control-label">出生日期</label>
              <div class="col-sm-7 date">
                <div class="input-group input-append date" id="datePicker">
                  <input type="text" class="form-control" name="date" />
                  <span class="input-group-addon add-on"><span class="glyphicon glyphicon-calendar"></span></span>
                </div>
              </div>
            </div>
            <div class="form-group">
              <label class="col-sm-3 control-label">验证码</label>
              <div class="col-sm-7">
                <input type="text" class="form-control" name="authCode" />
              </div>
            </div>
            <div class="form-group">
              <div class="col-sm-9 col-sm-offset-3">
                <!-- Do NOT use name="submit" or id="submit" for the Submit button -->
                <button type="submit" class="btn btn-default">注册</button>
                <button type="button" class="btn btn-default" id="resetButton">重置</button>
              </div>
            </div>
          </form>
        </div>
      </div>
    </div>
    <!-- /container -->
    <script>
      $(document).ready(function() {
        $('#datePicker')
          .datepicker({
            format: 'yyyy-mm-dd',
            language: 'zh-CN'
          })
          .on('changeDate', function(e) {
            // Revalidate the date field
            $('#registrationForm').bootstrapValidator('revalidateField', 'date');
          });

        $('#registrationForm').bootstrapValidator({
          feedbackIcons: {
            valid: 'glyphicon glyphicon-ok',
            invalid: 'glyphicon glyphicon-remove',
            validating: 'glyphicon glyphicon-refresh'
          },
          fields: {
            username: {
              validators: {
                notEmpty: {},
                stringLength: {
                  min: 6,
                  max: 30
                },
                regexp: {
                  regexp: /^[a-zA-Z0-9_]+$/,
                  message: '用户名中只能包含英文字母，数字与下划线'
                }
              }
            },
            email: {
              validators: {
                notEmpty: {},
                emailAddress: {}
              }
            },
            password: {
              validators: {
                notEmpty: {},
                stringLength: {
                  min: 8
                },
                identical: {
                  field: 'confirmPassword'
                }
              }
            },
            confirmPassword: {
              validators: {
                notEmpty: {},
                stringLength: {
                  min: 8
                },
                identical: {
                  field: 'password'
                }
              }
            },
            date: {
              validators: {
                notEmpty: {},
                date: {
                  format: 'YYYY-MM-DD'
                }
              }
            },
            authCode: {
              validators: {
                notEmpty: {}
              }
            }
          }
        });
        // Reset the Tooltip container form
        $('#resetButton').on('click', function(e) {
          // Then reset the form
          $('#registrationForm').data('bootstrapValidator').resetForm(true);
        });
      });
    </script>
  </body>
</html>