package iscas.entity;

public class News {

	private String title;//标题
	private String contentHref;//链接
	private String date;//日期
	
	public News(String string, String string2, String string3) {
		// TODO Auto-generated constructor stub
		title = string;
		contentHref = string2;
		date = string3;
	}
	
	public News() {
		super();
		// TODO Auto-generated constructor stub
	}
	
	public String getTitle() {
		return title;
	}
	public void setTitle(String title) {
		this.title = title;
	}
	public String getContentHref() {
		return contentHref;
	}
	public void setContentHref(String contentHref) {
		this.contentHref = contentHref;
	}
	public String getDate() {
		return date;
	}
	public void setDate(String date) {
		this.date = date;
	}
}
