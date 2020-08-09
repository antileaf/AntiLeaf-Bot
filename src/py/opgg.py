# Code by KSkun, 2020/5

import re
import requests
from lxml import etree

url = 'http://www.op.gg/champion/statistics'
headers = {
    'Accept-Language': 'zh' # 为了让得到的网页信息是中文的，设置 HTTP 头
}

if __name__ == '__main__':
    ans = []
    for posi in ["TOP", "JUNGLE", "MID", "ADC", "SUPPORT"]:
        page = requests.get(url, headers=headers).content.decode('utf-8')
        html = etree.HTML(page) # 加载 HTML
        tr_list = html.xpath('//tbody[@class=\"tabItem champion-trend-tier-' + posi + '\"]/tr') # 用 XPath 定位到要找的表
        for tr in tr_list: # 遍历表里的每一项
            # rank
            rank = int(tr.xpath('.//td[@class=\"champion-index-table__cell champion-index-table__cell--rank\"]/text()')[0]) # XPath 提取排名文本，转型成数字

            # diff
            diff_ele_list = [
                tr.xpath('.//td[@class=\"champion-index-table__cell champion-index-table__cell--change champion-index-table__cell--change--stay\"]'),
                tr.xpath('.//td[@class=\"champion-index-table__cell champion-index-table__cell--change champion-index-table__cell--change--up\"]'),
                tr.xpath('.//td[@class=\"champion-index-table__cell champion-index-table__cell--change champion-index-table__cell--change--down\"]')
            ]
            diff = "" # 因为页面上 up down stay 的形状是 image 展示的，只能用 class 值区分这些，然后手动提取数字和箭头拼接成结果，提取数字用了正则
            if len(diff_ele_list[0]) != 0:
                diff = "=" + re.findall(r'\d+', diff_ele_list[0][0].xpath("text()")[1])[0]
            elif len(diff_ele_list[1]) != 0:
                diff = "↑" + re.findall(r'\d+', diff_ele_list[1][0].xpath("text()")[1])[0]
            elif len(diff_ele_list[2]) != 0:
                diff = "↓" + re.findall(r'\d+', diff_ele_list[2][0].xpath("text()")[1])[0]
        
            # detail_page
            detail_page = tr.xpath('.//td[@class=\"champion-index-table__cell champion-index-table__cell--image\"]/a/@href')[0] # XPath，同上，下略
            english_name = detail_page.partition("/champion/")[2]\
                .partition("/statistics")[0]

            # name
            name = tr.xpath('.//div[@class=\"champion-index-table__name\"]/text()')[0]

            # position
            position = re.findall(r'[^\x00-\xff]+', tr.xpath('.//div[@class=\"champion-index-table__position\"]/text()')[0]) # 这用正则把所有双字节字符过滤了，原因是原始字符串里面有好多空白字符

            # win_rate
            win_rate = tr.xpath('.//td[@class=\"champion-index-table__cell champion-index-table__cell--value\"]/text()')[0]

            # attd_rate
            attd_rate = tr.xpath('.//td[@class=\"champion-index-table__cell champion-index-table__cell--value\"]/text()')[1]

            # level
            level = int(re.findall(r'\d+', tr.xpath('.//td[@class=\"champion-index-table__cell champion-index-table__cell--value\"]/img/@src')[0])[0]) # 因为页面上没找到能用的层级数值文本，只能从层级的 image 文件名提取了，用的也是正则

            # print(rank, diff, detail_page, name, position, win_rate, attd_rate, level)
            ans += [(english_name,name)]
    ans.sort()
    for i in range(len(ans)):
        if i == 0 or ans[i] != ans[i - 1]:
            print("{\"" + ans[i][0] + "\",", "\"" + ans[i][1] + "\", \"\"},")
    
    