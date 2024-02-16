import 'package:currency_convertor/model/rates_model.dart';
import 'package:currency_convertor/view/widgets/conversion_card.dart';
import 'package:currency_convertor/data/network/api_services.dart';
import 'package:flutter/material.dart';

class HomeScreen extends StatefulWidget {
  const HomeScreen({Key? key}) : super(key: key);

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  late Future<RatesModel> ratesModel;
  late Future<Map> currenciesModel;
  bool showMessage = false;

  @override
  void initState() {
    super.initState();
    _fetchData();
    _showMessageAfterDelay();
  }

  Future<void> _fetchData() async {
    ratesModel = fetchRates();
    currenciesModel = fetchCurrencies();
  }

  Future<void> _showMessageAfterDelay() async {
    await Future.delayed(const Duration(seconds: 2));
    setState(() {
      showMessage = true;
    });
    await Future.delayed(const Duration(seconds: 4));
    setState(() {
      showMessage = false;
    });
  }

  Future<void> _restartApp() async {
    Navigator.pushReplacement(
      context,
      MaterialPageRoute(builder: (BuildContext context) => const HomeScreen()),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      resizeToAvoidBottomInset: false,
      appBar: AppBar(
        leading: const Icon(Icons.currency_exchange),
        title: const Text('Currency Convertor'),
        actions: [
          IconButton(
            icon: const Icon(Icons.refresh),
            onPressed: _restartApp,
          ),
        ],
      ),
      body: FutureBuilder<RatesModel>(
        future: ratesModel,
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return const Center(
              child: CircularProgressIndicator(),
            );
          } else {
            return FutureBuilder<Map>(
              future: currenciesModel,
              builder: (context, index) {
                if (index.connectionState == ConnectionState.waiting) {
                  return const Center(
                    child: CircularProgressIndicator(),
                  );
                } else if (index.hasError) {
                  return Center(child: Text('Error: ${index.error}'));
                } else if (index.data == null) {
                  return const Center(child: Text('No data available'));
                } else {
                  return Column(
                    children: [
                      if (showMessage) // Show message conditionally
                        const Padding(
                          padding: EdgeInsets.all(8.0),
                          child: Text(
                            'You can turn off your mobile data..',
                            style: TextStyle(
                              color: Colors.green,
                              fontWeight: FontWeight.bold,
                            ),
                          ),
                        ),
                      Expanded(
                        child: ConversionCard(
                          rates: snapshot.data!.rates,
                          currencies: index.data!,
                        ),
                      ),
                    ],
                  );
                }
              },
            );
          }
        },
      ),
    );
  }
}
