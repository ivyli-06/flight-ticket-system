import { Router } from "express";
import { z } from "zod";
import { pool } from "../db.js";

export const flightsRouter = Router();

const querySchema = z.object({
  departure: z.string().min(1),
  arrival: z.string().min(1),
  date: z.string().regex(/^\d{4}-\d{2}-\d{2}$/) // YYYY-MM-DD
});

flightsRouter.get("/", async (req, res) => {
  const parsed = querySchema.safeParse(req.query);
  if (!parsed.success) return res.status(400).json({ error: "Invalid query params" });

  const { departure, arrival, date } = parsed.data;

  const [rows] = await pool.execute<any[]>(
    `SELECT id, flight_number, departure, arrival, departure_time, arrival_time, price, available_seats
     FROM flights
     WHERE departure = ? AND arrival = ? AND DATE(departure_time) = ?
     ORDER BY departure_time ASC`,
    [departure, arrival, date]
  );

  return res.json({ flights: rows });
});