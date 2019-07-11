Distribution-Centre Web API User Guide
======================================

Summary of API Requests
-----------------------

* \<host-domain\>/api/bins/contents/get.php?id=\<string\>
* \<host-domain\>/api/bins/item_counts/get.php
* \<host-domain\>/api/bins/positions/get.php
* \<host-domain\>/api/orders/place.php
* \<host-domain\>/api/orders/get.php
* \<host-domain\>/api/pickers/read.php
* \<host-domain\>/api/products/categories/get.php
* \<host-domain\>/api/products/page/get.php
* \<host-domain\>/api/products/page_count/get.php

API Request Details
-------------------

**\<host-domain\>/api/bins/contents/get.php?id=\<string\>**

Returns the contents of a stock bin, specified by ID. ID can be a valid stock bin ID, "shipping", or "receiving". Use request method GET.

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

**\<host-domain\>/api/bins/item_counts/get.php**

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

**\<host-domain\>/api/bins/positions/get.php**

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

**\<host-domain\>/api/orders/place.php**

Places an order with the distribution centre. Can specify whether the customer should be emailed upon order being received and/or shipped. Maximum of 12 items per order accepted. Maximum of 5 orders may be queued. Use request method POST.

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
		"SUCCESS",
		"ERROR_invalid_order_request_format",
		"ERROR_server_side",
		"ERROR_customer_info_refused",
		"ERROR_insufficient_stock",
		"ERROR_maximum_items_exceeded",
		"ERROR_maximum_orders_exceeded"
		*/
		// The following are only returned upon success.
		"orderId": <string>
	}

---

**\<host-domain\>/api/orders/get.php**

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

**\<host-domain\>/api/products/categories/get.php**

Returns a list of all valid product categories. Use request method GET.

	[
		<string>,
		...
	]

---

**\<host-domain\>/api/products/page/get.php**

Returns a page of products. Use request method POST.

The body of the request must take the following format:

	{
		"page": <int>,
		"limit": <int>,
		"category": <string>,
		"quantity": <bool>,
		"description": <bool>,
		"image_url": <bool>,
		"price": <bool>
	}

The body of the response will take the following format:

	{
		"status": <string>,
		/*
		"status" may be one of:
		"SUCCESS",
		"ERROR_invalid_limit",
		"ERROR_invalid_category",
		"ERROR_invalid_page",
		"ERROR_invalid_page_request_format"
		*/
		// The following are only returned upon success.
		"limit": <int>,
		"page": <int>,
		"category": <string>,
		"products": [
			{
				"name": <string>,
				// Only returned if quantity set true.
				"quantity": <int>,
				// Only returned if description set true.
				"description": <string>,
				// Only returned if image_url set true.
      			"image_url": <string>,
				// Only returned if price set true.
				// String format is "####.##".
      			"price": <string>
			},
			...
		]
	}

---

**\<host-domain\>/api/products/page_count/get.php**

Returns the number of pages needed to list all of the products. A valid category must be provided, or "all". A limit on the number of items per page msu be provided. Use request method POST.

The body of the request must take the following format:

	{
		"limit": <int>,
		"category": <string>
	}

The body of the response will take the following format:

	{
		"status": <bool>,
		// The following is only returned upon failure.
		"error": <string>,
		/*
		"error" may be one of:
		"invalid_page_count_request_format"
		"invalid_limit",
		"invalid_category"
		*/
		// The following are only returned upon success.
		"limit": <int>,
		"pages": <int>,
		"category": <string>
	}
