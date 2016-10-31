/*
//重写时间转换函数
Date.prototype.toLocaleString = function() {
	return this.getFullYear() + "年" + (this.getMonth() + 1) + "月" + this.getDate() + "日 " + this.getHours() + ":" + this.getMinutes() + ":" + this.getSeconds();
};
*/
document.getElementById('digitaltime').innerHTML = new Date().toLocaleString() + ' 星期' + '日一二三四五六'.charAt(new Date().getDay());
setInterval("document.getElementById('digitaltime').innerHTML=new Date().toLocaleString()+' 星期'+'日一二三四五六'.charAt(new Date().getDay());", 1000);
