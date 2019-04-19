Automated-Order-Picker
======================

Summary
-------

This is a simplified simulation of an automated distribution centre. An SQL database (MariaDB) maintains a list of products,
locations in the warehouse, order-queue, and products in shipping/stock/receiving. A C++ program simulates a series of automated
order pickers that handle incoming stock in receiving, and move items from stock to shipping to satisfy the current order. A web
interface is provided for viewing the warehouse. An API will be provided to allow anyone to create a front-end to interact with the
warehouse and place fictitious orders.

Todo
----
- Implement web API
- Finalize available products and descriptions
- Warehouse needs to send email on order submission/completion
- Tidy up warehouse web view, make responsive for mobile

Contact
-------

*Author*: Steven Joshua Klickermann\
*Email*: sjklickermann@gmail.com\
*GitHub*: https://github.com/sjklick

License
-------

This project is licensed under the terms of the GNU General Public
License v3.0 (see LICENSE.md).
