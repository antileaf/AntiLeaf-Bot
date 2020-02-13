import os
import traceback

import requests

from lxml import etree


URL = ''
def get_html_doc(url):
    response = requests.get(url, headers=headers)
    response.encoding = "UTF-8"
    return etree.HTML(response.text) #压成特定结构

def spy_info(html_doc):
	dicts = {}
	url_elements = html_doc.xpath('//tr/td[1]/a') # Xpath 先过一层
	for a in url_elements:
		number = str(a.text)  # 允许套娃，./a 等效于 //table/tr/td[1]/a
		link = str(a.xpath('./@href')[0]) # 有些缺德网站会整相对/绝对路径，注意
		fulllink = "https://oeis.org" + link
		dicts.update({number: fulllink})
	return dicts


if __name__ == '__main__':
	seq = input() # 要搜索的序列
	URL = 'https://oeis.org/search?q=' + seq
	headers = {
		'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.87 Safari/537.36',
		'Host': 'oeis.org',
		'Referer': 'https://oeis.org/',
	}
	html_doc = get_html_doc(URL)
	ret = spy_info(html_doc)

	f = open('oeis.txt', 'w')
	for (a, b) in ret.items():
		print(a, ' ', b, file = f)
	f.close()