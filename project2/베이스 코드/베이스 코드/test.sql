CREATE TABLE Address
( 
	Address_ID         int  NOT NULL,
	street             varchar(20)  NOT NULL,
	city               varchar(20)  NOT NULL,
	state_name         varchar(20)  NOT NULL,
	country            varchar(20)  NOT NULL,
	type               varchar(20)  NOT NULL 
);

CREATE TABLE Address_Info
( 
	destination_Address_ID int  NOT NULL,
	origin_Address_ID  int  NOT NULL,
	service_type       varchar(20)  NOT NULL,
	estimated_delivery_date datetime  NOT NULL 
);

CREATE TABLE Billing
( 
	Customer_ID        int  NOT NULL,
	Package_ID         int  NOT NULL,
	billing_date       datetime  NOT NULL,
	amount             decimal(10,2)  NOT NULL,
	bill_type          varchar(20)  NOT NULL 
);

CREATE TABLE Content
( 
	Content_type       varchar(20)  NOT NULL,
	content_value      int  NOT NULL,
	hazard_status      varchar(20)  NOT NULL,
	type               varchar(20)  NOT NULL,
	weight             int  NOT NULL 
);

CREATE TABLE Customer
( 
	Customer_ID        int  NOT NULL,
	name               varchar(20)  NOT NULL,
	phone              varchar(20)  NOT NULL,
	email              varchar(20)  NOT NULL,
	account_type       varchar(20)  NOT NULL,
	account_number     varchar(20)  NOT NULL,
	payment_method     varchar(20)  NOT NULL,
	credit_card_info   varchar(20)  NOT NULL,
	Address_ID         int  NULL 
);

CREATE TABLE Package
( 
	Package_ID         int  NOT NULL,
	destination_Address_ID int  NOT NULL,
	origin_Address_ID  int  NOT NULL,
	recipient_name     varchar(20)  NOT NULL,
	status             varchar(20)  NOT NULL,
	Customer_ID        int  NOT NULL,
	Content_type       varchar(20)  NULL 
);

CREATE TABLE Shipment
( 
	Shipment_ID        int  NOT NULL,
	shipment_date      datetime  NOT NULL,
	Package_ID         int  NULL,
	Transport_ID       int  NULL,
	delivered_status   varchar(20)  NOT NULL,
	delivered_date     datetime  NOT NULL 
);
