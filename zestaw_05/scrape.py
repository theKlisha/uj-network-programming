import sys
import json 
import requests
from bs4 import BeautifulSoup

def getHtml():
    baseUrl = "https://store.steampowered.com/search/results/"
    queryParams = {
        "query": None,
        "start": 0,
        "count": 100,
        "ignore_preferences": 1,
        "filter": "topsellers",
    }

    return requests.get(baseUrl, queryParams).text

def scrapePrice(rowEl):
    try:
        discountEl = rowEl.find(class_="search_discount")
        searchPriceEl = rowEl.find(class_="search_price")

        if searchPriceEl.string != None and searchPriceEl.string.strip() == "Free to Play":
            return {
                "isFreeToPlay": True,
            }
        elif discountEl.span != None:
            return {
                "isDiscounted": True,
                "discountPercent": discountEl.span.string.strip(),
                "amountBefore": searchPriceEl.span.strike.string.strip(),
                "amount": list(searchPriceEl.children)[3].strip()
            }
        elif searchPriceEl.string != None and searchPriceEl.string.strip() != "":
            return {
                "amount": searchPriceEl.string.strip()
            }
        else:
            return None
    except:
        return None

def scrapeItem(rowEl):
    item = {
        "id": rowEl["data-ds-itemkey"].replace("App_", ""),
        "title": rowEl.find(class_="title").string,
        "releaseDate": rowEl.find(class_="search_released").string,
        "storeUrl": rowEl["href"],
        "imageUrl": rowEl.find("img")["src"],
        "price": scrapePrice(rowEl)
    }
    
    return item

def scrapeItems(data):
    soup = BeautifulSoup(data, features="html.parser")
    rowEls = soup.find(id="search_resultsRows").find_all(class_="search_result_row")
    items = []

    for rowEl in rowEls:
        items.append(scrapeItem(rowEl))
    
    return items

try:
    html = getHtml()
    items = scrapeItems(html)
    jsonOut = json.dumps(items) 
    sys.stdout.write(jsonOut)
    sys.exit(0);
except:
    sys.exit(1);
