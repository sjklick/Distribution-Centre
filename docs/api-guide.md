Distribution-Centre Web API User Guide
======================================

Summary of API Requests
-----------------------

* \<host-domain\>/api/bin_item_counts/read.php
* \<host-domain\>/api/bin_positions/read.php
* \<host-domain\>/api/bin/read.php?id=\<string\>
* \<host-domain\>/api/categories/read.php
* \<host-domain\>/api/order/place.php
* \<host-domain\>/api/orders/read.php
* \<host-domain\>/api/pickers/read.php
* \<host-domain\>/api/products/page/get.php?category=\<string\>&limit=\<int\>&page=\<int\>
* \<host-domain\>/api/products/page_count/get.php?category=\<string\>&limit=\<int\>
* \<host-domain\>/api/products/read.php
* \<host-domain\>/api/products/read.php?category=\<string\>

API Request Details
-------------------

**\<host-domain\>/api/bin_item_counts/read.php**

Returns the total count of items (0-12) in each bin. Use request method GET.

	{
		"stock": [
			<int>,
			...
  		],
  		"receiving": <int>,
		"shipping": <int>
	}

---

**\<host-domain\>/api/bin_positions/read.php**

Returns all stock bin positions, ordered by bin ID. Use request method GET.

	[
		{
			"row": <int>,
			"column": <int>,
			"facing": <string>
		},
		...
	]

---

**\<host-domain\>/api/bin/read.php?id=\<string\>**

Returns the contents of a stock bin, specified by ID. ID is can be a valid stock bin ID, "shipping", or "receiving". Use request method GET.

	{
		"id": <string>,
		"item": [
			{
				"name": <string>,
				"quantity": <int>
			},
			...
		]
	}

---

**\<host-domain\>/api/categories/read.php**

Returns a list of all valid product categories. Use request method GET.

	[
		<string>,
		...
	]

---

**\<host-domain\>/api/order/place.php**

Places an order with the distribution centre. Can specify whether the customer should be emailed upon order being received and/or shipped. Use request method POST.

The body of the request must take the following format:

	{
    	"customer": <string>,
		"email": <string>,
		"confirmReceived": <bool>,
		"confirmShipped": <bool>,
		"items":
		[
			{
				"name": <string>,
				"quantity": <int>
			},
			...
		]
	}

The body of the response will take the following format:

	{
		"status": <string>,
		/*
		"status" may be one of:
		"ERROR_customer_info_refused"
		"ERROR_invalid_order_request_format"
		"ERROR_insufficient_stock"
		"ERROR_server_side"
		"SUCCESS_order_placed"
		*/
		// The following are only returned upon success.
		"orderId": <string>
	}

---

**\<host-domain\>/api/orders/read.php**

Returns the order IDs of all placed orders, starting with the order currently being processed. Use request method GET.

	[
		<int>,
		...
	]

---

**\<host-domain\>/api/pickers/read.php**

Returns information regarding all automated order pickers including position, state, and whether or not an item is being transported. Use request method GET.

	[  
		{  
			"row": <int>,  
			"column": <int>,  
			"facing": <string>,  
			"state": <string>,  
			"item": <bool>  
		},  
		...  
	]

---

**\<host-domain\>/api/products/page/get.php?category=\<string\>&limit=\<int\>&page=\<int\>**

Returns a page of products. An optional category may be provided, otherwise "all" is assumed. An optional limit on the number of items per page may be provided, otherwise 10 is assumed. An optional page may be given, otherwise the first page is assumed. Use request method GET.

	{
		"status": <string>,
		"limit": <int>,
		"page": <int>,
		"category": <string>,
		"products": [
			{
				"name": <string>,
				"quantity": <string>
			},
			...
		]
	}

---

**\<host-domain\>/api/products/page_count/get.php?category=\<string\>&limit=\<int\>**

Returns the number of pages needed to list all of the products. An optional category may be provided, otherwise "all" is assumed. An optional limit on the number of items per page may be provided, otherwise 10 is assumed. Use request method GET.

	{
		"status": <string>,
		/*
		"status" may be one of:
		"SUCCESS",
		"ERROR_invalid_limit",
		"ERROR_invalid_category"
		*/
		// The following are only returned upon success.
		"limit": <int>,
		"pages": <int>,
		"category": <string>
	}

---

**\<host-domain\>/api/products/read.php**

Returns all of the products, including name and quantity, from every category. Use request method GET.

	[
		{
			"name": <string>,
			"quantity": <string>
		},
		...
	]

---

**\<host-domain\>/api/products/read.php?category=\<string\>**

Returns all of the products, including name and quantity, from a valid category. Use request method GET.

	[
		{
			"name": <string>,
			"quantity": <string>
		},
		...
	]