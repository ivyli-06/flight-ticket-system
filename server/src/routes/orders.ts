import { Router } from "express";
import { pool } from "../db.js";
import { authRequired } from "../middleware/authRequired.js";

export const ordersRouter = Router();

ordersRouter.get("/", authRequired, async (req, res) => {
  const userId = req.user!.id;

  const [rows] = await pool.execute<any[]>(
    `SELECT o.id,
            o.status,
            o.order_time,
            f.flight_number,
            f.departure,
            f.arrival,
            f.departure_time,
            f.arrival_time
     FROM orders o
     JOIN flights f ON o.flight_id = f.id
     WHERE o.user_id = ?
     ORDER BY o.order_time DESC`,
    [userId]
  );

  return res.json({ orders: rows });
});

ordersRouter.post("/:id/cancel", authRequired, async (req, res) => {
  const userId = req.user!.id;
  const orderId = Number(req.params.id);
  if (!Number.isFinite(orderId)) return res.status(400).json({ error: "Invalid order id" });

  // 只允许取消“属于自己”的订单
  const [result] = await pool.execute<any>(
    "UPDATE orders SET status = 'cancelled' WHERE id = ? AND user_id = ?",
    [orderId, userId]
  );

  // mysql2 result 结构随版本略有差异，这里宽松处理
  const affected = (result?.affectedRows ?? result?.[0]?.affectedRows ?? 0) as number;
  if (!affected) return res.status(404).json({ error: "Order not found" });

  return res.json({ ok: true });
});