INSERT INTO time_wh
SELECT
	nid as pm_key,
	datum AS Date,
	to_char(datum, 'TMDay') AS WeekdayName,
	extract(day FROM datum) AS Day,
	cast (to_char(datum, 'WW') as int) AS Week,
	extract(month FROM datum) AS Month,
	extract(year FROM datum) AS Year,
	cast(to_char(datum, 'Q') as int) AS Quarter,
	-- Fixed holidays 
        CASE WHEN to_char(datum, 'MMDD') IN ('0101', '0704', '1225', '1111')
		THEN 't' ELSE 'f' END
		AS Holiday_flag,
	-- Weekend
	CASE WHEN extract(isodow FROM datum) IN (6, 7) THEN 'f' ELSE 't' END AS Weekday_flag
FROM (
	SELECT '1992-01-01'::DATE + sequence.day AS datum, sequence.day as nid
	FROM generate_series(0,3650) AS sequence(day)
	GROUP BY sequence.day
     ) DQ
ORDER BY 1
