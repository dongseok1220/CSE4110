#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "mysql.h"

#pragma comment(lib, "libmysql.lib")

const char* host = "localhost";
const char* user = "root";
const char* pw = "mysql";
const char* db = "project";

void execute_and_print_query(MYSQL* connection, const char* query) {
	if (mysql_query(connection, query) == 0) {
		MYSQL_RES* result = mysql_store_result(connection);
		if (result != NULL) {
			MYSQL_ROW row;
			unsigned int num_fields = mysql_num_fields(result);
			MYSQL_FIELD* fields = mysql_fetch_fields(result);

			while ((row = mysql_fetch_row(result)) != NULL) {
				for (unsigned int i = 0; i < num_fields; i++) {
					printf("%s : %s \n", fields[i].name, row[i] ? row[i] : "NULL");
				}
				printf("\n");
			}
		}
	}
	else {
		printf("Error executing query: %s\n", mysql_error(connection));
	}
}

void execute_query_type_I(MYSQL* connection) {
	char query[1000];
	int ID, type;
	printf("---- TYPE I ----\n\n");
	printf("Input the Transport_ID : ");
	scanf("%d", &ID);
	if (ID == 0) return;

	printf("\n---- Subtypes in TYPE I ----\n");
	printf("\t1. TYPE I-1\n");
	printf("\t2. TYPE I-2\n");
	printf("\t3. TYPE I-3\n");

	printf("Select Type: ");
	scanf("%d", &type);
	printf("\n\n");
	getchar();
	if (type == 0) return;
	if (type == 1) {
		printf("\n---- TYPE I-1: Find all customers who had a package on the truck at the time of the crash. ----\n");
		sprintf(query, "SELECT DISTINCT c.name FROM Customer AS c INNER JOIN Package p ON c.Customer_ID = p.Customer_ID INNER JOIN Shipment s ON p.Package_ID = s.Package_ID INNER JOIN Transport t ON s.Transport_ID = t.Transport_ID WHERE t.Transport_ID = %d AND t.status = 'accident' AND s.delivered_status = 'no'", ID);
		execute_and_print_query(connection, query);
	}
	else if (type == 2) {
		printf("\n---- TYPE I-2: Find all recipients who had a package on that truck at the time of the crash. ----\n");
		sprintf(query, "SELECT DISTINCT p.recipient_name FROM Customer AS c INNER JOIN Package p ON c.Customer_ID = p.Customer_ID INNER JOIN Shipment s ON p.Package_ID = s.Package_ID INNER JOIN Transport t ON s.Transport_ID = t.Transport_ID WHERE t.Transport_ID = %d AND t.status = 'accident' AND s.delivered_status = 'no'", ID);
		execute_and_print_query(connection, query);
	}
	else {
		printf("\n---- TYPE I-3: Find the last successful delivery by that truck prior to the crash. ----\n");
		sprintf(query, "SELECT s.Package_ID INTO @last_delivered_package_id FROM Shipment AS s WHERE s.Transport_ID = %d AND s.delivered_status = 'yes' ORDER BY s.delivered_date DESC LIMIT 1;", ID);
		if (mysql_query(connection, query) != 0) {
			printf("Error executing query: %s\n", mysql_error(connection));
			return;
		}
		sprintf(query, "SELECT p.* FROM Package AS p WHERE p.Package_ID = @last_delivered_package_id;");
		execute_and_print_query(connection, query);
	}
	printf("\n");
}

void execute_query_type_II(MYSQL* connection) {
	char query[1000];
	int year;
	printf("Year ? : ");
	scanf("%d", &year);
	if (year == 0) return;
	printf("\n---- TYPE II: Find the customer who has shipped the most packages in the past year ----\n");
	sprintf(query, "SELECT c.Customer_ID, c.name, COUNT(p.Package_ID) AS num_packages FROM Package p JOIN Shipment s ON p.Package_ID = s.Package_ID JOIN Customer c ON c.Customer_ID = p.Customer_ID WHERE YEAR(s.shipment_date) = %d GROUP BY c.Customer_ID ORDER BY num_packages DESC LIMIT 1", year);
	execute_and_print_query(connection, query);
}

void execute_query_type_III(MYSQL* connection) {
	char query[1000];
	int year;
	printf("Year ? : ");
	scanf("%d", &year);
	if (year == 0) return; 
	printf("\n---- TYPE III: Find the customer who has spent the most money on shipping in the past year ----\n");
	sprintf(query, "SELECT b.Customer_ID, c.name, SUM(b.amount) AS total_amount FROM Billing b JOIN Customer c ON b.Customer_ID = c.Customer_ID WHERE YEAR(b.billing_date) = %d GROUP BY b.Customer_ID ORDER BY total_amount DESC LIMIT 1", year);
	execute_and_print_query(connection, query);
}

void execute_query_type_IV(MYSQL* connection) {
	char query[1000];
	printf("\n---- TYPE IV: Find the packages that were not delivered within the promised time ----\n");
	sprintf(query, "SELECT p.Package_ID, s.delivered_date, a.estmated_delivery_date FROM Package p JOIN Shipment s ON p.Package_ID = s.Package_ID JOIN Address_Info a ON p.destination_Address_ID = a.destination_Address_ID AND p.origin_Address_ID = a.origin_Address_ID WHERE s.delivered_date > a.estmated_delivery_date");
	execute_and_print_query(connection, query);
}

void execute_query_type_V(MYSQL* connection, int billType) {
	char query[1000];
	int customer_id, year, month;

	int express = 200;
	int international = 250;
	int domestic = 100;

	int truck = 50;
	int ship = 100;
	int airplane = 300;

	printf("\n---- TYPE V: Generate the bill for each customer for the past month ----\n");
	printf("Customer ID ? : ");
	scanf("%d", &customer_id);
	if (customer_id == 0) return;

	printf("Year ? : ");
	scanf("%d", &year);
	if (year == 0) return;

	printf("Month ? : ");
	scanf("%d", &month);
	if (month == 0) return;
	printf("\n----------------------------------\n");

	// Query for Customer Info
	printf("Customer Info\n");
	sprintf(query, "SELECT * FROM Customer WHERE Customer_ID = %d", customer_id);
	execute_and_print_query(connection, query);


	// Query for Address Info
	printf("\nAddress Info\n");
	sprintf(query, "SELECT a.* FROM Address a JOIN Customer c ON a.Address_ID = c.Address_ID WHERE c.Customer_ID = %d", customer_id);
	execute_and_print_query(connection, query);


	if (billType == 1) {
		// Simple Bill
		printf("\nSimple Bill\n");
		sprintf(query,
			"SELECT SUM(b.amount "
			"+ CASE a.service_type WHEN 'express' THEN %d WHEN 'international' THEN %d WHEN 'domestic' THEN %d ELSE 0 END "
			"+ CASE t.type WHEN 'truck' THEN %d WHEN 'ship' THEN %d WHEN 'airplane' THEN %d ELSE 0 END) as total "
			"FROM Billing b "
			"JOIN Package p ON b.Package_ID = p.Package_ID "
			"JOIN Address_Info a ON p.destination_Address_ID = a.destination_Address_ID and p.origin_Address_ID = a.origin_Address_ID "
			"JOIN Shipment s ON b.Package_ID = s.Package_ID "
			"JOIN Transport t ON s.Transport_ID = t.Transport_ID "
			"WHERE b.Customer_ID = %d AND YEAR(b.billing_date) = %d AND MONTH(b.billing_date) = %d", express, international, domestic, truck, ship, airplane, customer_id, year, month);

	}
	else if (billType == 2) {
		// Bill by Type of Service
		printf("\nBill by Type of Service\n");
		sprintf(query,
			"SELECT a.service_type, GROUP_CONCAT(b.Package_ID SEPARATOR ', ') as Package_IDs, SUM(b.amount "
			"+ CASE a.service_type WHEN 'express' THEN %d WHEN 'international' THEN %d WHEN 'domestic' THEN %d ELSE 0 END "
			"+ CASE t.type WHEN 'truck' THEN %d WHEN 'ship' THEN %d WHEN 'airplane' THEN %d ELSE 0 END) as total "
			"FROM Billing b "
			"JOIN Package p ON b.Package_ID = p.Package_ID "
			"JOIN Address_Info a ON p.destination_Address_ID = a.destination_Address_ID and p.origin_Address_ID = a.origin_Address_ID "
			"JOIN Shipment s ON b.Package_ID = s.Package_ID "
			"JOIN Transport t ON s.Transport_ID = t.Transport_ID "
			"WHERE b.Customer_ID = %d AND YEAR(b.billing_date) = %d AND MONTH(b.billing_date) = %d "
			"GROUP BY a.service_type", express, international, domestic, truck, ship, airplane, customer_id, year, month);
	}
	else {
		// Itemized Billing
		printf("\nItemized Billing\n");
		sprintf(query,
			"SELECT b.Package_ID, a.service_type, t.type, SUM(b.amount "
			"+ CASE a.service_type WHEN 'express' THEN %d WHEN 'international' THEN %d WHEN 'domestic' THEN %d ELSE 0 END "
			"+ CASE t.type WHEN 'truck' THEN %d WHEN 'ship' THEN %d WHEN 'airplane' THEN %d ELSE 0 END) as total "
			"FROM Billing b "
			"JOIN Package p ON b.Package_ID = p.Package_ID "
			"JOIN Address_Info a ON p.destination_Address_ID = a.destination_Address_ID and p.origin_Address_ID = a.origin_Address_ID "
			"JOIN Shipment s ON b.Package_ID = s.Package_ID "
			"JOIN Transport t ON s.Transport_ID = t.Transport_ID "
			"WHERE b.Customer_ID = %d AND YEAR(b.billing_date) = %d AND MONTH(b.billing_date) = %d "
			"GROUP BY b.Package_ID, a.service_type, t.type", express, international, domestic, truck, ship, airplane, customer_id, year, month);
	}
	execute_and_print_query(connection, query);
	
}



int main(void) {

	MYSQL* connection = NULL;
	MYSQL conn;
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;

	if (mysql_init(&conn) == NULL)
		printf("mysql_init() error!");

	connection = mysql_real_connect(&conn, host, user, pw, db, 3306, (const char*)NULL, 0);
	if (connection == NULL)
	{
		printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}

	else
	{
		printf("Connection Succeed\n");

		if (mysql_select_db(&conn, db))
		{
			printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
			return 1;
		}

		// Open SQL file and read the query
		FILE* file = fopen("20191619.txt", "r");
		if (file == NULL) {
			printf("Cannot open file\n");
			return 1;
		}

		while (!feof(file))
		{
			char strTemp[1000];
			if (fgets(strTemp, sizeof(strTemp), file) != NULL)
			{	
				// printf("%s", strTemp);
				if (mysql_query(connection, strTemp)) {
					fprintf(stderr, "%s\n", mysql_error(connection));
					exit(1);
				}
			}
		}
		fclose(file);


		int type; 

		while (1) {
			printf("------- SELECT QUERY TYPES -------\n\n");
			printf("\t1. TYPE I\n");
			printf("\t2. TYPE II\n");
			printf("\t3. TYPE III\n");
			printf("\t4. TYPE IV\n");
			printf("\t5. TYPE V\n");
			printf("\t0. QUIT\n");
			printf("\n----------------------------------\n");
			printf("Select Type: ");
			scanf("%d", &type);
			printf("\n\n");
			getchar();

			if (type == 0) {
				break;
			}
			else if (type == 1) {
				execute_query_type_I(connection);
			}
			else if (type == 2) {
				execute_query_type_II(connection);
			}
			else if (type == 3) {
				execute_query_type_III(connection);
			}
			else if (type == 4) {
				execute_query_type_IV(connection);
			}
			else if (type == 5) {
				int billType;
				printf("Select the bill type:\n");
				printf("\t1. Simple Bill\n");
				printf("\t2. Bill by Type of Service\n");
				printf("\t3. Itemized Billing\n");
				printf("Select Type: ");
				scanf("%d", &billType);
				printf("\n\n");
				getchar();
				if (billType == 0) continue; 
				execute_query_type_V(connection, billType);
			}
		}

		FILE* file2 = fopen("20191619_2.txt", "r");
		if (file2 == NULL) {
			printf("Cannot open file\n");
			return 1;
		}

		while (!feof(file2))
		{
			char strTemp[1000];
			if (fgets(strTemp, sizeof(strTemp), file2) != NULL)
			{
				// printf("%s", strTemp);
				if (mysql_query(connection, strTemp)) {
					fprintf(stderr, "%s\n", mysql_error(connection));
					exit(1);
				}
			}
		}
		fclose(file2);
		printf("Drop Success!");
		mysql_close(connection);
	}
	return 0;
}
