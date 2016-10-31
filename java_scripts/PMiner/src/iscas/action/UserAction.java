package iscas.action;

import java.io.UnsupportedEncodingException;

import javax.annotation.Resource;
import javax.servlet.ServletRequest;

import iscas.entity.User;
import iscas.service.UserService;

import org.apache.shiro.SecurityUtils;
import org.apache.shiro.authc.*;
import org.apache.shiro.authz.annotation.RequiresPermissions;
import org.apache.shiro.subject.Subject;
import org.apache.struts2.ServletActionContext;
import org.apache.struts2.convention.annotation.Action;
import org.apache.struts2.convention.annotation.ParentPackage;
import org.apache.struts2.convention.annotation.Result;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Controller;

import com.opensymphony.xwork2.ActionSupport;

@Controller("UserAction")
@Scope("prototype")
@ParentPackage("struts-default")
public class UserAction extends ActionSupport {

	/**
	 * 用户处理类
	 */
	private static final long serialVersionUID = -2689736136098568594L;
	
	private static final Logger logger = LoggerFactory.getLogger(UserAction.class);
	
	@Resource
	private UserService userService;
	
	private String username;
	private String password;
	private boolean rememberMe;
	
	@Action(value="login", results={@Result(name=ERROR, location="/login-error.jsp"),
			@Result(name=SUCCESS, location="/index.html")})
	public String login() {
		Subject currentUser = SecurityUtils.getSubject();
		AuthenticationToken token =	new UsernamePasswordToken(username, password, rememberMe);
		ServletRequest request = ServletActionContext.getRequest();
		try {
			request.setCharacterEncoding("utf-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		
		try {
            currentUser.login(token); // 使用当前subject尝试登陆操作
        } catch (UnknownAccountException e) {
        	addActionMessage("用户名不存在");
            logger.error("用户名不存在：{}", username);
            request.setAttribute("error", "用户名不存在");
            return ERROR;
        } catch (IncorrectCredentialsException e) {
        	addActionMessage("密码错误");
            logger.error("密码错误：{}", username);
            request.setAttribute("error", "密码错误");
            return ERROR;
        } catch (LockedAccountException e) {
        	addActionMessage("账号已被锁定");
            logger.error("账号已被锁定：{}", username);
            request.setAttribute("error", "账号已被锁定");
            return ERROR;
        } catch (ExcessiveAttemptsException eae ) {
        	addActionMessage("登陆已超过重试次数");
            logger.error("登陆已超过重试次数：{}", username);
            request.setAttribute("error", "登陆已超过重试次数");
            return ERROR;
        } catch (AuthenticationException e) {
            addActionMessage("用户名或密码错误");
            logger.error("登录失败：{}", username);
            request.setAttribute("error", "用户名或密码错误");
            return ERROR;
        }
        return SUCCESS;
	}
	
	@Action(value="logout", results={@Result(name=LOGIN, location="/login.jsp")})
	public String logout() throws Exception {
		Subject currentUser = SecurityUtils.getSubject();
		currentUser.logout();
        return LOGIN;
	}
	
	@Action(value="register", results= {@Result(name=SUCCESS, location="/login.jsp")})
	public String register() {
		User user = new User(username, password);
		userService.createUser(user);
		return SUCCESS;
	}
	
	@RequiresPermissions("user:edit")
	@Action(value="update", results={@Result(name=SUCCESS, type="redirect", location="list.action")})
	public String update() {
		System.out.println("update............");
		return SUCCESS;
	}

	public String getUsername() {
		return username;
	}

	public void setUsername(String username) {
		this.username = username;
	}

	public String getPassword() {
		return password;
	}

	public boolean getRememberMe() {
		return rememberMe;
	}

	public void setPassword(String password) {
		this.password = password;
	}

	public void setRememberMe(boolean rememberMe) {
		this.rememberMe = rememberMe;
	}

	public UserService getUserService() {
		return userService;
	}

	public void setUserService(UserService userService) {
		this.userService = userService;
	}
}
