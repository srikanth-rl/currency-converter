import 'package:currency_convertor/view/home_screen.dart';
import 'package:flutter/material.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Currency Convertor',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(
            seedColor: Colors.deepPurple, brightness: Brightness.light),
        useMaterial3: true,
        fontFamily: 'Lato',
        textTheme: const TextTheme(
          // Define the default TextStyle for the entire app
          bodyLarge: TextStyle(
            fontSize: 26, // Set the default font size
            fontFamily: 'Lato', // Set the default font family
          ),
          // You can define more styles for different text types if needed
        ),
      ),
      home: const HomeScreen(),
    );
  }
}
