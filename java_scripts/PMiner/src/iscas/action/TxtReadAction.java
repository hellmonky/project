package iscas.action;

import iscas.entity.News;
import iscas.service.DataProcess;

import java.io.IOException;
import java.util.List;

import net.sf.json.JSONArray;

import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Controller;

import com.opensymphony.xwork2.ActionSupport;

@Controller
@Scope("prototype")
public class TxtReadAction extends ActionSupport  {
	
	private static final long serialVersionUID = 6942057571624739595L;
	
	private JSONArray jsonWReadNews = null;
	private JSONArray jsonWReadAppoint = null;
	
	public JSONArray getJsonWReadAppoint() {
		return jsonWReadAppoint;
	}
	public void setJsonWReadAppoint(JSONArray jsonWReadAppoint) {
		this.jsonWReadAppoint = jsonWReadAppoint;
	}
	public JSONArray getJsonWReadNews() {
		return jsonWReadNews;
	}
	public void setJsonWReadNews(JSONArray jsonWReadNews) {
		this.jsonWReadNews = jsonWReadNews;
	}
	
	public String readNewsTxt() throws IOException {
		List<News> newsList = DataProcess.getNewsList();
		jsonWReadNews = JSONArray.fromObject(newsList);
		return SUCCESS;
	}
	
	public String readAppointTxt() throws IOException {
		List<News> newsList = DataProcess.getAppointList();
		jsonWReadAppoint = JSONArray.fromObject(newsList);
		return SUCCESS;
	}
}
