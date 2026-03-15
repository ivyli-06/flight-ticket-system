import mysql from "mysql2/promise";
import "dotenv/config";

const port = Number(process.env.DB_PORT ?? 3306);

export const pool = mysql.createPool({
  host: process.env.DB_HOST ?? "127.0.0.1",
  port,
  user: process.env.DB_USER ?? "root",
  password: process.env.DB_PASSWORD ?? "",
  database: process.env.DB_NAME ?? "test_db",
  waitForConnections: true,
  connectionLimit: 10,
  namedPlaceholders: true
});